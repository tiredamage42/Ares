#include "AresPCH.h"
#include "SceneRenderer.h"

#include "Renderer.h"
#include "Ares/Core/Scene.h"
#include "Ares/Core/Entity.h"
#include "Ares/Core/Components.h"
#include "Ares/Core/Application.h"
#include "Ares/Renderer/Renderer2D.h"
#include "Ares/Math/Math.h"
#include <glad/glad.h>

/*
	TODO:
	remove editor specific stuff
*/


namespace Ares {

	struct ShaderVariationPair
	{
		Ref<Shader> Shader;
		ShaderVariations Variant;

		// requred to compare in hashmap
		bool operator==(const ShaderVariationPair& o) const
		{
			return o.Shader == Shader && o.Variant == Variant;
		}
	};
}

namespace std {
	template <>
	struct hash<Ares::ShaderVariationPair>
	{
		std::size_t operator()(const Ares::ShaderVariationPair& k) const
		{
			return hash<uint32_t>()(k.Shader->GetRendererID(k.Variant));
		}
	};
}

namespace Ares {

	struct MeshDrawCall
	{
		Ref<Mesh> Mesh;
		size_t SubmeshIndex;
		glm::mat4 Transform;
		bool Outlined;
		Ref<Texture2D> BoneTransforms;
	};

	typedef std::vector<MeshDrawCall> MeshDrawCalls;
	typedef std::unordered_map<Ref<Material>, MeshDrawCalls> MaterialsMap;
	typedef std::unordered_map<ShaderVariationPair, MaterialsMap> RenderMap;

	struct SceneRendererData
	{ 
		const Scene* ActiveScene = nullptr;
		
		struct SceneInfo
		{
			SceneRendererCamera SceneCamera;
			float Exposure;
			Ref<Material> SkyboxMaterial;
			Light ActiveLight;
		} SceneData;

		Ref<Shader> CompositeShader;

		Ref<RenderPass> GeoPass;
		Ref<RenderPass> CompositePass;

		struct DrawCommand
		{
			Ref<Mesh> Mesh;
			glm::mat4 Transform;
			Ref<Texture2D> BoneTransforms;
		};

		RenderMap UnlitDrawMap;
		RenderMap FwdBaseDrawMap;
		RenderMap FwdAddDrawMap;
		std::vector<DrawCommand> SelectedMeshDrawList;
		std::vector<DrawCommand> DrawList;

		Ref<Shader> ErrorShader;
		Ref<Shader> OutlineShader;

		SceneRendererOptions Options;
	};

	static SceneRendererData s_Data;


	static void DrawRenderMapUnlit(const RenderMap& renderMap, const glm::mat4& viewProjection, const glm::mat4& view)
	{
		ARES_PROFILE_FUNCTION();

		// Read our shaders into the appropriate buffers
		for (auto& shader_materials : renderMap)
		{
			const ShaderVariationPair& shaderVarPair = shader_materials.first;
			Ref<Shader> shader = shaderVarPair.Shader;
			ShaderVariations variant = shaderVarPair.Variant;

			shader->Bind(variant);
			shader->SetMat4("ares_VPMatrix", viewProjection);
			shader->SetMat4("ares_VMatrix", view);
			
			const MaterialsMap& materials = shader_materials.second;
			for (auto& materials_draws : materials)
			{
				Ref<Material> material = materials_draws.first;
				const MeshDrawCalls& draws = materials_draws.second;
				material->Bind();
				for (auto& dc : draws)
				{
					if (dc.Outlined)
					{
						// enable writing to the stencil buffer
						// We clear the stencil buffer to 0s at the start of the frame and for the outlined object 
						// we update the stencil buffer to 1 for each fragment drawn:
						Renderer::Submit([]() { glStencilMask(0xff); }, "outline other stuff");
					}
						
					Renderer::SubmitMesh(shader, dc.Mesh, dc.Transform, dc.SubmeshIndex, dc.BoneTransforms, material->GetFlag(MaterialFlag::DepthTest));

					if (dc.Outlined)
					{
						// make sure we don't update the stencil buffer while drawing the floor
						Renderer::Submit([]() { glStencilMask(0x00); }, "Outline Stencil mask");
					}
				}
			}
		}
	}


	static void DrawRenderMapForwardBase(const RenderMap& renderMap, const glm::mat4& viewProjection, const glm::mat4& view)
	{
		ARES_PROFILE_FUNCTION();

		// Read our shaders into the appropriate buffers
		for (auto& shader_materials : renderMap)
		{
			const ShaderVariationPair& shaderVarPair = shader_materials.first;
			Ref<Shader> shader = shaderVarPair.Shader;
			ShaderVariations variant = shaderVarPair.Variant;

			shader->Bind(variant);
			shader->SetMat4("ares_VPMatrix", viewProjection);
			shader->SetMat4("ares_VMatrix", view);
			
			const MaterialsMap& materials = shader_materials.second;
			for (auto& materials_draws : materials)
			{
				Ref<Material> material = materials_draws.first;
				const MeshDrawCalls& draws = materials_draws.second;
				material->Bind();

				bool depthTest = material->GetFlag(MaterialFlag::DepthTest);
				// foreach light (if shader is lit)
				{
					shader->SetFloat3("ares_Light.Direction", s_Data.SceneData.ActiveLight.Direction);
					shader->SetFloat3("ares_Light.Color", s_Data.SceneData.ActiveLight.Radiance * s_Data.SceneData.ActiveLight.Multiplier);
					
					for (auto& dc : draws)
					{
						if (dc.Outlined)
						{
							// enable writing to the stencil buffer
							// We clear the stencil buffer to 0s at the start of the frame and for the outlined object 
							// we update the stencil buffer to 1 for each fragment drawn:
							Renderer::Submit([]() { glStencilMask(0xff); }, "outline other stuff");
						}
							
						Renderer::SubmitMesh(shader, dc.Mesh, dc.Transform, dc.SubmeshIndex, dc.BoneTransforms, depthTest);
							
						if (dc.Outlined)
						{
							// make sure we don't update the stencil buffer while drawing the floor
							Renderer::Submit([]() { glStencilMask(0x00); }, "Outline Stencil mask");
						}
					}
				}
			}
		}
	}

	static void DrawRenderMapForwardAdd(const RenderMap& renderMap, const glm::mat4& viewProjection, const glm::mat4& view)
	{
		ARES_PROFILE_FUNCTION();

		// Read our shaders into the appropriate buffers
		for (auto& shader_materials : renderMap)
		{
			const ShaderVariationPair& shaderVarPair = shader_materials.first;
			Ref<Shader> shader = shaderVarPair.Shader;
			ShaderVariations variant = shaderVarPair.Variant;

			shader->Bind(variant);
			shader->SetMat4("ares_VPMatrix", viewProjection);
			shader->SetMat4("ares_VMatrix", view);
			
			const MaterialsMap& materials = shader_materials.second;
			for (auto& materials_draws : materials)
			{
				Ref<Material> material = materials_draws.first;
				const MeshDrawCalls& draws = materials_draws.second;
				material->Bind();

				bool depthTest = material->GetFlag(MaterialFlag::DepthTest);
				// foreach light (if shader is lit)
				{
					
					shader->SetFloat3("ares_Light.Direction", s_Data.SceneData.ActiveLight.Direction);
					shader->SetFloat3("ares_Light.Color", s_Data.SceneData.ActiveLight.Radiance * s_Data.SceneData.ActiveLight.Multiplier);

					for (auto& dc : draws)
					{
						Renderer::SubmitMesh(shader, dc.Mesh, dc.Transform, dc.SubmeshIndex, dc.BoneTransforms, depthTest);
					}
				}
			}
		}
	}

	

	
	void SceneRenderer::Init()
	{

		FrameBufferSpecs geoFramebufferSpec;
		geoFramebufferSpec.Width = 1280;
		geoFramebufferSpec.Height = 720;
		geoFramebufferSpec.Format = FramebufferFormat::RGBA16F;
		geoFramebufferSpec.Samples = 8;
		geoFramebufferSpec.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

		RenderPassSpecs geoRenderPassSpec;
		geoRenderPassSpec.TargetFrameBuffer = FrameBuffer::Create(geoFramebufferSpec);
		s_Data.GeoPass = CreateRef<RenderPass>(geoRenderPassSpec);

		FrameBufferSpecs compFramebufferSpec;
		compFramebufferSpec.Width = 1280;
		compFramebufferSpec.Height = 720;
		compFramebufferSpec.Format = FramebufferFormat::RGBA8;
		compFramebufferSpec.ClearColor = { 0.5f, 0.1f, 0.1f, 1.0f };

		RenderPassSpecs compRenderPassSpec;
		compRenderPassSpec.TargetFrameBuffer = FrameBuffer::Create(compFramebufferSpec);
		s_Data.CompositePass = CreateRef<RenderPass>(compRenderPassSpec);
		s_Data.CompositeShader = Shader::Find("Assets/Shaders/HDRTonemapping.glsl");
		
		
		s_Data.OutlineShader = Shader::Find("Assets/Shaders/UnlitColor.glsl");
		s_Data.ErrorShader = Shader::Find("Assets/Shaders/ErrorShader.glsl");

	}

	void SceneRenderer::Shutdown()
	{
		
	}

	void SceneRenderer::SetViewportSize(uint32_t width, uint32_t height)
	{
		Ref<FrameBuffer> fBufferGeo = s_Data.GeoPass->GetSpecs().TargetFrameBuffer;

		fBufferGeo->Resize(width, height);

		s_Data.CompositePass->GetSpecs().TargetFrameBuffer->Resize(width, height);
	}

	void SceneRenderer::BeginScene(const Scene* scene, const SceneRendererCamera& camera)
	{

		ARES_CORE_ASSERT(!s_Data.ActiveScene, "");

		s_Data.ActiveScene = scene;

		s_Data.SceneData.SceneCamera = camera;
		s_Data.SceneData.SkyboxMaterial = scene->m_SkyboxMaterial;
		s_Data.SceneData.ActiveLight = scene->m_Light;

		s_Data.SceneData.Exposure = scene->m_Exposure;		
	}

	void SceneRenderer::EndScene()
	{

		ARES_CORE_ASSERT(s_Data.ActiveScene, "");

		s_Data.ActiveScene = nullptr;

		FlushDrawList();
	}
	void SceneRenderer::FlushDrawList()
	{
		ARES_PROFILE_FUNCTION();

		ARES_CORE_ASSERT(!s_Data.ActiveScene, "");
		GeometryPass();
		CompositePass();

		s_Data.UnlitDrawMap.clear();
		s_Data.FwdBaseDrawMap.clear();
		s_Data.FwdAddDrawMap.clear();

		s_Data.DrawList.clear();
		s_Data.SelectedMeshDrawList.clear();

		s_Data.SceneData = {};
	}

	static void AddToDrawMap(RenderMap& drawMap, Ref<Shader> shader, ShaderVariations shaderVariant, Ref<Material> material, Ref<Mesh> mesh, uint32_t submeshIndex, const glm::mat4& transform, Ref<Texture2D> boneTransforms, bool outlined)
	{
		ShaderVariationPair shaderVarPair = { shader, shaderVariant };

		if (drawMap.find(shaderVarPair) == drawMap.end())
			drawMap[shaderVarPair] = MaterialsMap();

		MaterialsMap& materialsMap = drawMap.at(shaderVarPair);

		if (materialsMap.find(material) == materialsMap.end())
			materialsMap[material] = MeshDrawCalls();

		MeshDrawCalls& drawCalls = materialsMap.at(material);
		drawCalls.push_back({ mesh, submeshIndex, transform, outlined, boneTransforms });
	}

	void SceneRenderer::SubmitMesh(Ref<Mesh> mesh, const glm::mat4& transform, Ref<Texture2D> boneTransforms, std::vector<Ref<Material>> materials, bool isSelected)
	{

		if (isSelected)
		{
			s_Data.SelectedMeshDrawList.push_back({ mesh, transform, boneTransforms });
		}
		s_Data.DrawList.push_back({ mesh, transform, boneTransforms });

		bool useSkinning = boneTransforms != nullptr;

		auto& submeshes = mesh->GetSubmeshes();
		for (uint32_t i = 0; i < submeshes.size(); i++)
		{
			Ref<Material> material = materials[submeshes[i].MaterialIndex];
			Ref<Shader> shader = material->GetShader();

			bool isLit = shader->IsLit();

			if (isLit)
			{
				if (!shader->HasVariation(ShaderVariations::ForwardBase))
				{
					ARES_CORE_WARN("ForwardBase Pass: Shader '{0}' is Flagged as lit but has no Forward Base Pass", shader->GetName());
					AddToDrawMap(s_Data.UnlitDrawMap, s_Data.ErrorShader, useSkinning ? ShaderVariations::DefaultSkinned : ShaderVariations::Default, material, mesh, i, transform, boneTransforms, isSelected);
					continue;
				}
				else
				{
					ShaderVariations shaderVariant = useSkinning ? ShaderVariations::ForwardBaseSkinned : ShaderVariations::ForwardBase;
					AddToDrawMap(s_Data.FwdBaseDrawMap, shader, shaderVariant, material, mesh, i, transform, boneTransforms, isSelected);
				}

				// TODO: only do this if we have more than one light
				if (!shader->HasVariation(ShaderVariations::ForwardAdd))
				{
					//ARES_CORE_WARN("ForwardAdd Pass: Shader '{0}' is Flagged as lit but has no Forward Add Pass", shader->GetName());
				}
				else
				{
					ShaderVariations shaderVariant = useSkinning ? ShaderVariations::ForwardAddSkinned : ShaderVariations::ForwardAdd;
					AddToDrawMap(s_Data.FwdAddDrawMap, shader, shaderVariant, material, mesh, i, transform, boneTransforms, isSelected);
				}
			}
			else
			{
				if (!shader->HasVariation(ShaderVariations::Default))
				{
					ARES_CORE_WARN("Unlit Pass: Shader '{0}' is Flagged as unlit but has no Default pass", shader->GetName());
					shader = s_Data.ErrorShader;
				}

				AddToDrawMap(s_Data.UnlitDrawMap, shader, useSkinning ? ShaderVariations::DefaultSkinned : ShaderVariations::Default, material, mesh, i, transform, boneTransforms, isSelected);
			}
		}	
	}


	
	
	

	Ref<TextureCube> SceneRenderer::ConvertHDRToCubemap(const std::string& filepath, uint32_t cubemapSize)
	{
		// laod teh 2d equirectangular map
		Ref<Texture2D> envEquirect = Texture2D::Create(filepath, FilterType::Trilinear, false);
		ARES_CORE_ASSERT(envEquirect->GetFormat() == TextureFormat::Float16, "Texture is not HDR!");
		
		// create the target cubemap
		Ref<TextureCube> cubemap = TextureCube::Create(TextureFormat::Float16, cubemapSize, cubemapSize, FilterType::Trilinear, false);
		Shader::Find("Assets/Shaders/EquirectangularToCubeMap.glsl")->Bind(ShaderVariations::Default);
		envEquirect->Bind();
		Renderer::Submit([cubemap, cubemapSize, envEquirect]() {
			glBindImageTexture(0, cubemap->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
			glDispatchCompute(cubemapSize / 32, cubemapSize / 32, 6);
		}, "");
		return cubemap;
	}

	


	static void DrawBoundingBoxes(const std::vector<SceneRendererData::DrawCommand>& drawList, bool depthTest)
	{
		for (auto& dc : drawList)
			Renderer::DrawAABB(dc.Mesh, dc.Transform, s_Data.Options.AABBColor, depthTest);
	}

	static void SetupSelectedMeshDrawing()
	{
		Renderer::Submit([]() {

			/*
				By using GL_REPLACE as the stencil op function we make sure that each of the containers'
				fragments update the stencil buffer with a stencil value of 1.
				Because the fragments always pass the stencil test, the stencil buffer is updated
				with the reference value wherever we've drawn them.
			*/

			glStencilOp(
				// action to take if the stencil test fails.
				GL_KEEP,	// If any of the tests fail we do nothing; we keep the currently stored value 
							// that is in the stencil buffer. 
				// action to take if the stencil test passes, but the depth test fails.
				GL_REPLACE,
				// action to take if both the stenciland the depth test pass
				GL_REPLACE	// If both the stencil test and the depth test succeed however, 
							// we want to replace the stored stencil value with the reference value set 
							// via glStencilFunc which we later set to 1.
			);

			// all fragments should pass the stencil test
			glStencilFunc(GL_ALWAYS, 1, 0xff);

			// make sure we don't update the stencil buffer while drawing the normal geometry
			glStencilMask(0x00);

		}, "Outline Start");
	}

	static void DrawSelectedMeshes(const Matrix4& viewProjection)
	{

		Renderer::Submit([]() {
			glStencilMask(0x00); // disable writing to the stencil buffer
			glLineWidth(2);
			glEnable(GL_LINE_SMOOTH);
			glPolygonMode(GL_FRONT, GL_LINE);
			}, "Draw Outline Prepare");


		size_t outlineDrawCount = s_Data.SelectedMeshDrawList.size();
		std::vector<SceneRendererData::DrawCommand> outlineDraws;
		outlineDraws.resize(s_Data.SelectedMeshDrawList.size());
		size_t x = 0;
		size_t y = outlineDrawCount - 1;
		for (auto& dc : s_Data.SelectedMeshDrawList)
		{
			if (dc.BoneTransforms)
				outlineDraws[y--] = dc;
			else
				outlineDraws[x++] = dc;
		}
		size_t skinnedVariationStart = x;


		/*
			draw the mesh wireframe of the selected objects
		*/

		s_Data.OutlineShader->Bind(ShaderVariations::Default);
		s_Data.OutlineShader->SetMat4("ares_VPMatrix", viewProjection);
		s_Data.OutlineShader->SetFloat4("u_Color", s_Data.Options.MeshTrianglesColor);

		for (size_t i = 0; i < outlineDrawCount; i++)
		{
			if (i == skinnedVariationStart)
			{
				s_Data.OutlineShader->Bind(ShaderVariations::DefaultSkinned);
				s_Data.OutlineShader->SetMat4("ares_VPMatrix", viewProjection);
				s_Data.OutlineShader->SetFloat4("u_Color", s_Data.Options.MeshTrianglesColor);
			}

			Renderer::SubmitMesh(s_Data.OutlineShader, outlineDraws[i].Mesh, outlineDraws[i].Transform, outlineDraws[i].BoneTransforms, true);
		}


		/*
			Now that the stencil buffer is updated with 1s where the containers were drawn
			we're going to draw the upscaled containers, but this time with the appropriate
			test function and disabling writes to the stencil buffer:

			We set the stencil function to GL_NOTEQUAL to make sure that we're only drawing parts
			of the containers that are not equal to 1. This way we only draw the part of the containers
			that are outside the previously drawn containers. Note that we also disable depth testing so
			the scaled up containers (e.g. the borders) do not get overwritten by the floor.
		*/
		Renderer::Submit([]() {
			glStencilFunc(GL_NOTEQUAL, 1, 0xff);
			glLineWidth(10);
			}, "");

		s_Data.OutlineShader->Bind(ShaderVariations::Default);
		s_Data.OutlineShader->SetFloat4("u_Color", s_Data.Options.OutlineColor);

		for (size_t i = 0; i < outlineDrawCount; i++)
		{
			if (i == skinnedVariationStart)
			{
				s_Data.OutlineShader->Bind(ShaderVariations::DefaultSkinned);
				s_Data.OutlineShader->SetFloat4("u_Color", s_Data.Options.OutlineColor);
			}

			Renderer::SubmitMesh(s_Data.OutlineShader, outlineDraws[i].Mesh, outlineDraws[i].Transform, outlineDraws[i].BoneTransforms, false);
		}


		/*
			draw with points (this fills out gaps from the line rendering)
		*/
		Renderer::Submit([]() {
			glPointSize(10);
			glPolygonMode(GL_FRONT, GL_POINT);
			}, "");

		s_Data.OutlineShader->Bind(ShaderVariations::Default);
		for (size_t i = 0; i < outlineDrawCount; i++)
		{
			if (i == skinnedVariationStart)
				s_Data.OutlineShader->Bind(ShaderVariations::DefaultSkinned);

			Renderer::SubmitMesh(s_Data.OutlineShader, outlineDraws[i].Mesh, outlineDraws[i].Transform, outlineDraws[i].BoneTransforms, false);
		}

		Renderer::Submit([]() {
			glStencilMask(0xff);
			glStencilFunc(GL_ALWAYS, 1, 0xff);
			glPolygonMode(GL_FRONT, GL_FILL);
			glEnable(GL_DEPTH_TEST);
			}, "Outline End");


		/*
			draw the bounding boxes of the selected meshes
		*/
		DrawBoundingBoxes(s_Data.SelectedMeshDrawList, false);		
	}
	
	static void DrawGrid(const Vector3& cameraPosition)
	{
		Vector3 camPos = cameraPosition;
		camPos.y = 0;
		camPos.x = (int)camPos.x;
		camPos.z = (int)camPos.z;

		uint32_t gridRes = s_Data.Options.GridResolution;

		for (int i = -gridRes; i <= gridRes; i++)
		{
			Renderer2D::SubmitLine(camPos + Vector3{ i, 0, -gridRes }, camPos + Vector3{ i, 0, gridRes }, s_Data.Options.GridColor, true, s_Data.Options.GridCameraRange);
			Renderer2D::SubmitLine(camPos + Vector3{ -gridRes, 0, i }, camPos + Vector3{ gridRes, 0, i }, s_Data.Options.GridColor, true, s_Data.Options.GridCameraRange);
		}
	}
	
	void SceneRenderer::GeometryPass()
	{
		ARES_PROFILE_FUNCTION();

		auto viewMatrix = s_Data.SceneData.SceneCamera.ViewMatrix;
		auto viewProjection = s_Data.SceneData.SceneCamera.Camera.GetProjectionMatrix() * viewMatrix;
		glm::vec3 cameraPosition = glm::inverse(s_Data.SceneData.SceneCamera.ViewMatrix)[3];
		
		Renderer::BeginRenderPass(s_Data.GeoPass, true, true, true);
		
		Renderer2D::BeginScene(viewProjection, cameraPosition);
		
		bool outline = s_Data.SelectedMeshDrawList.size() > 0;
		if (outline)
		{
			SetupSelectedMeshDrawing();
		}

		DrawRenderMapUnlit(s_Data.UnlitDrawMap, viewProjection, viewMatrix);
		DrawRenderMapForwardBase(s_Data.FwdBaseDrawMap, viewProjection, viewMatrix);
		//DrawRenderMapForwardAdd(s_Data.FwdAddDrawMap, viewProjection, viewMatrix);


		if (outline)
		{
			DrawSelectedMeshes(viewProjection);
		}

		if (GetOptions().ShowBoundingBoxes)
		{
			DrawBoundingBoxes(s_Data.DrawList, true);
		}

		if (GetOptions().ShowGrid)
		{
			DrawGrid(cameraPosition);
		}

		// Skybox (rendered last to prevent overdraw)
		Renderer::Submit([]() { glDepthMask(false);	}, "");
		s_Data.SceneData.SkyboxMaterial->GetShader()->Bind(ShaderVariations::Default);
		s_Data.SceneData.SkyboxMaterial->GetShader()->SetMat4("ares_InverseVP", glm::inverse(viewProjection));
		Renderer::SubmitFullscreenQuad(s_Data.SceneData.SkyboxMaterial, true);
		Renderer::Submit([]() { glDepthMask(true); }, "");


		Renderer2D::EndScene();
		Renderer::EndRenderPass();
	}


	void SceneRenderer::CompositePass()
	{
		ARES_PROFILE_FUNCTION();

		Renderer::Submit([]() { glDepthMask(false); }, "");
		
		Renderer::BeginRenderPass(s_Data.CompositePass, true, true, true);
		s_Data.CompositeShader->Bind(ShaderVariations::Default);
		s_Data.CompositeShader->SetFloat("u_Exposure", s_Data.SceneData.Exposure);
		s_Data.CompositeShader->SetInt("u_TextureSamples", s_Data.GeoPass->GetSpecs().TargetFrameBuffer->GetSpecs().Samples);

		s_Data.GeoPass->GetSpecs().TargetFrameBuffer->BindAsTexture();
		Renderer::SubmitFullscreenQuad(nullptr, false);
		Renderer::EndRenderPass();
		
		Renderer::Submit([]() { glDepthMask(true); }, "");
	}

	Ref<RenderPass> SceneRenderer::GetFinalRenderPass()
	{
		return s_Data.CompositePass;
	}
	Ref<RenderPass> SceneRenderer::GetGeometryPass()
	{
		return s_Data.GeoPass;
	}

	uint32_t SceneRenderer::GetFinalColorBufferRendererID()
	{
		return s_Data.CompositePass->GetSpecs().TargetFrameBuffer->GetColorAttachmentRendererID();
	}
	SceneRendererOptions& SceneRenderer::GetOptions()
	{
		return s_Data.Options;
	}

}