#include "AresPCH.h"
#include "SceneRenderer.h"

#include "Renderer.h"
#include "Ares/Core/Entity.h"
#include "Ares/Core/Components.h"
#include "Ares/Renderer/Renderer2D.h"
#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>

namespace Ares {



//#define GRID_RESOLUTION 2147483646
#define GRID_RESOLUTION 100
#define GRID_WIDTH .05f

	struct SceneRendererData
	{ 
		const Scene* ActiveScene = nullptr;
		struct SceneInfo
		{
			SceneRendererCamera SceneCamera;

			// Resources
			float SkyboxLod, Exposure;
			Ref<MaterialInstance> SkyboxMaterial;
			Environment SceneEnvironment;
			Light ActiveLight;
		} SceneData;

		Ref<Texture2D> BRDFLUT;
		Ref<Shader> CompositeShader;

		Ref<RenderPass> GeoPass;
		Ref<RenderPass> CompositePass;

		struct DrawCommand
		{
			Ref<Mesh> Mesh;
			Ref<MaterialInstance> MaterialOverride;
			glm::mat4 Transform;
		};
		std::vector<DrawCommand> DrawList;
		std::vector<DrawCommand> SelectedMeshDrawList;

		// Grid
		Ref<MaterialInstance> GridMaterial;
		Ref<MaterialInstance> OutlineMaterial;

		SceneRendererOptions Options;
	};

	static SceneRendererData s_Data;

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

		s_Data.CompositeShader = Shader::Find("Assets/Shaders/hdr.glsl");
		s_Data.BRDFLUT = Texture2D::Create("Assets/Textures/BRDF_LUT.tga", FilterType::Point, false);

		// Grid
		auto gridShader = Shader::Find("Assets/Shaders/grid.glsl");

		s_Data.GridMaterial = CreateRef<MaterialInstance>(CreateRef<Material>(gridShader));

		// TODO: SET SCENE VALUES
		/*float gridScale = 16.025f, gridSize = 0.025f;
		s_Data.GridMaterial->Set("u_Scale", gridScale);
		s_Data.GridMaterial->Set("u_Res", gridSize);*/

		//s_Data.GridMaterial->Set("u_MVP", viewProjection * glm::scale(glm::mat4(1.0f), glm::vec3(m_GridScale - m_GridSize)));
				
		s_Data.GridMaterial->Set("u_Scale", (float)GRID_RESOLUTION);
		s_Data.GridMaterial->Set("u_Res", GRID_WIDTH);

		// Outline
		auto outlineShader = Shader::Find("Assets/Shaders/Outline.glsl");
		s_Data.OutlineMaterial = CreateRef<MaterialInstance>(CreateRef<Material>(outlineShader));
		s_Data.OutlineMaterial->SetFlag(MaterialFlag::DepthTest, false);

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
		s_Data.SceneData.SceneEnvironment = scene->m_Environment;
		s_Data.SceneData.ActiveLight = scene->m_Light;

		s_Data.SceneData.Exposure = scene->m_Exposure;
		s_Data.SceneData.SkyboxLod = scene->m_SkyboxLod;
	}

	void SceneRenderer::EndScene()
	{

		ARES_CORE_ASSERT(s_Data.ActiveScene, "");

		s_Data.ActiveScene = nullptr;

		FlushDrawList();
	}
	void SceneRenderer::FlushDrawList()
	{
		ARES_CORE_ASSERT(!s_Data.ActiveScene, "");

		GeometryPass();
		CompositePass();

		s_Data.DrawList.clear();
		s_Data.SelectedMeshDrawList.clear();

		s_Data.SceneData = {};
	}

	void SceneRenderer::SubmitMesh(Ref<Mesh> mesh, const glm::mat4& transform, Ref<MaterialInstance> materialOverrides)

	//void SceneRenderer::SubmitEntity(Entity& entity)
	{
		// TODO: Culling, sorting, etc.

		/*auto& mrComponent = entity.GetComponent<MeshRendererComponent>();
		if (!mrComponent.Mesh)
			return;*/
		
		s_Data.DrawList.push_back({ 
			mesh, 
			materialOverrides,
			transform
		});
	}
	void SceneRenderer::SubmitSelectedMesh(Ref<Mesh> mesh, const glm::mat4& transform)
	{
		s_Data.SelectedMeshDrawList.push_back({ mesh, nullptr, transform });
	}

	static Ref<Shader> equirectangularConversionShader, envFilteringShader, envIrradianceShader;

	std::pair<Ref<TextureCube>, Ref<TextureCube>> SceneRenderer::CreateEnvironmentMap(const std::string& filepath)
	{
		const uint32_t cubemapSize = 2048;
		//const uint32_t irradianceMapSize = 2048;
		const uint32_t irradianceMapSize = 32;

		Ref<TextureCube> envUnfiltered = TextureCube::Create(TextureFormat::Float16, cubemapSize, cubemapSize, FilterType::Trilinear, true);
		
		if (!equirectangularConversionShader)
			equirectangularConversionShader = Shader::Find("Assets/Shaders/EquirectangularToCubeMap.glsl");
		
		Ref<Texture2D> envEquirect = Texture2D::Create(filepath, FilterType::Trilinear, true);

		ARES_CORE_ASSERT(envEquirect->GetFormat() == TextureFormat::Float16, "Texture is not HDR!");
		 
		equirectangularConversionShader->Bind();
		envEquirect->Bind();
		Renderer::Submit([envUnfiltered, cubemapSize, envEquirect](){
				glBindImageTexture(0, envUnfiltered->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
				glDispatchCompute(cubemapSize / 32, cubemapSize / 32, 6);
				glGenerateTextureMipmap(envUnfiltered->GetRendererID());
			}, "EnvMap 0");
		
		//envUnfiltered->GenerateMipMaps();


		if (!envFilteringShader)
			envFilteringShader = Shader::Find("Assets/Shaders/EnvironmentMipFilter.glsl");

		Ref<TextureCube> envFiltered = TextureCube::Create(TextureFormat::Float16, cubemapSize, cubemapSize, FilterType::Trilinear, true);

		Renderer::Submit([envUnfiltered, envFiltered](){
				glCopyImageSubData(
					envUnfiltered->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
					envFiltered->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
					envFiltered->GetWidth(), envFiltered->GetHeight(), 6);
			}, "EnvMap 1");


		envFilteringShader->Bind();
		envUnfiltered->Bind();

		Renderer::Submit([envUnfiltered, envFiltered, cubemapSize]() {
			const float deltaRoughness = 1.0f / glm::max((float)(envFiltered->GetMipLevelCount() - 1.0f), 1.0f);
			for (int level = 1, size = cubemapSize / 2; level < envFiltered->GetMipLevelCount(); level++, size /= 2) // <= ?
			{
				const GLuint numGroups = glm::max(1, size / 32);
				glBindImageTexture(0, envFiltered->GetRendererID(), level, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
				glProgramUniform1f(envFilteringShader->GetRendererID(), 0, level * deltaRoughness);
				glDispatchCompute(numGroups, numGroups, 6);
			}
			}, "EnvMap 2");

		if (!envIrradianceShader)
			envIrradianceShader = Shader::Find("Assets/Shaders/EnvironmentIrradiance.glsl");

		Ref<TextureCube> irradianceMap = TextureCube::Create(TextureFormat::Float16, irradianceMapSize, irradianceMapSize, FilterType::Trilinear, true);
		envIrradianceShader->Bind();
		envFiltered->Bind();
		Renderer::Submit([irradianceMap, irradianceMapSize](){
				glBindImageTexture(0, irradianceMap->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
				glDispatchCompute(irradianceMapSize / 32, irradianceMapSize / 32, 6);
				//glDispatchCompute(irradianceMapSize, irradianceMapSize, 6);
				glGenerateTextureMipmap(irradianceMap->GetRendererID());
			}, "EnvMap 3");

		//irradianceMap->GenerateMipMaps();

		return { envFiltered, irradianceMap };
	}

	void SceneRenderer::GeometryPass()
	{

		bool outline = s_Data.SelectedMeshDrawList.size() > 0;

		if (outline)
		{
			Renderer::Submit([](){
				glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			}, "Outline STart");
		}

		Renderer::BeginRenderPass(s_Data.GeoPass);

		//auto viewProjection = s_Data.SceneData.SceneCamera.GetProjectionMatrix() * s_Data.SceneData.SceneCamera.GetViewMatrix();
		//auto viewProjection = s_Data.SceneData.SceneCamera.GetViewProjection();

		if (outline)
		{
			Renderer::Submit([](){
				glStencilMask(0);
			}, "Outline Stencil mask");
		}

		auto viewProjection = s_Data.SceneData.SceneCamera.Camera.GetProjectionMatrix() * s_Data.SceneData.SceneCamera.ViewMatrix;
		glm::vec3 cameraPosition = glm::inverse(s_Data.SceneData.SceneCamera.ViewMatrix)[3];


		// Skybox
		//auto skyboxShader = s_Data.SceneData.SkyboxMaterial->GetShader();
		s_Data.SceneData.SkyboxMaterial->Set("u_InverseVP", glm::inverse(viewProjection));

		//float skyboxLod = s_Data.ActiveScene->GetSkyboxLod();
		s_Data.SceneData.SkyboxMaterial->Set("u_TextureLod", s_Data.SceneData.SkyboxLod);

		// s_Data.SceneInfo.EnvironmentIrradianceMap->Bind(0);
		Renderer::SubmitFullscreenQuad(s_Data.SceneData.SkyboxMaterial);

		// TODO: render order based on material....
		// Render entities
		for (auto& dc : s_Data.DrawList)
		{
			auto baseMaterial = dc.Mesh->GetMaterial();
		//	auto baseMaterial = dc.Material;// Mesh->GetMaterial();
			baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
			baseMaterial->Set("u_CameraPosition", cameraPosition);

			// Environment (TODO: don't do this per mesh)
			baseMaterial->Set("u_EnvRadianceTex", s_Data.SceneData.SceneEnvironment.RadianceMap);
			baseMaterial->Set("u_EnvIrradianceTex", s_Data.SceneData.SceneEnvironment.IrradianceMap);
			baseMaterial->Set("u_BRDFLUTTexture", s_Data.BRDFLUT);

			//ARES_CORE_LOG("LGIHT {0}", s_Data.SceneData.ActiveLight.Multiplier);
			// Set lights (TODO: move to light environment and don't do per mesh)
			baseMaterial->Set("lights", s_Data.SceneData.ActiveLight);

			auto overrideMaterial = nullptr; // dc.Material;
			Renderer::SubmitMesh(dc.Mesh, dc.Transform, overrideMaterial);
		//	//auto overrideMaterial = nullptr; // dc.Material;

		//	//baseMaterial->Bind();
			//Renderer::SubmitMesh(dc.Mesh, dc.Transform);// , baseMaterial->GetShader());
		//	//Renderer::SubmitMesh(dc.Mesh, dc.Transform, overrideMaterial);
		}

		if (outline)
		{
			Renderer::Submit([](){
					glStencilFunc(GL_ALWAYS, 1, 0xff);
					glStencilMask(0xff);
				}, "outline other stuff");
		}

		for (auto& dc : s_Data.SelectedMeshDrawList)
		{
			auto baseMaterial = dc.Mesh->GetMaterial();
			baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
			baseMaterial->Set("u_CameraPosition", cameraPosition);

			// Environment (TODO: don't do this per mesh)
			baseMaterial->Set("u_EnvRadianceTex", s_Data.SceneData.SceneEnvironment.RadianceMap);
			baseMaterial->Set("u_EnvIrradianceTex", s_Data.SceneData.SceneEnvironment.IrradianceMap);
			baseMaterial->Set("u_BRDFLUTTexture", s_Data.BRDFLUT);

			// Set lights (TODO: move to light environment and don't do per mesh)
			baseMaterial->Set("lights", s_Data.SceneData.ActiveLight);

			auto overrideMaterial = nullptr; // dc.Material;
			Renderer::SubmitMesh(dc.Mesh, dc.Transform, overrideMaterial);
		}

		if (outline)
		{
			Renderer::Submit([](){
					glStencilFunc(GL_NOTEQUAL, 1, 0xff);
					glStencilMask(0);

					glLineWidth(10);
					glEnable(GL_LINE_SMOOTH);
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glDisable(GL_DEPTH_TEST);
				}, "Draw Outline Prepare");

			// Draw outline here
			s_Data.OutlineMaterial->Set("u_ViewProjection", viewProjection);
			for (auto& dc : s_Data.SelectedMeshDrawList)
			{
				Renderer::SubmitMesh(dc.Mesh, dc.Transform, s_Data.OutlineMaterial);
			}

			Renderer::Submit([](){
					glPointSize(10);
					glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				}, "Draw Outline polymode");
			for (auto& dc : s_Data.SelectedMeshDrawList)
			{
				Renderer::SubmitMesh(dc.Mesh, dc.Transform, s_Data.OutlineMaterial);
			}

			Renderer::Submit([](){
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glStencilMask(0xff);
					glStencilFunc(GL_ALWAYS, 1, 0xff);
					glEnable(GL_DEPTH_TEST);
				}, "Outline End");
		}


		// Grid
		if (GetOptions().ShowGrid)
		{


			//int m_GridScale = 8;
			//float m_GridSize = 0.025f;
			//float m_GridSize = 0.5f;

			s_Data.GridMaterial->Set("u_ViewProjection", viewProjection);


			//s_Data.GridMaterial->Set("u_MVP", viewProjection * glm::scale(glm::mat4(1.0f), glm::vec3(m_GridScale - m_GridSize)));

			// TODO: get scale from scene params
			//Renderer::SubmitQuad(s_Data.GridMaterial, glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(16.0f)));
			//Renderer::SubmitQuad(s_Data.GridMaterial, glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(m_GridScale - m_GridSize)));
			
			Renderer::SubmitQuad(s_Data.GridMaterial, glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(GRID_RESOLUTION * .5f + GRID_WIDTH * .5f)));
		}

		if (GetOptions().ShowBoundingBoxes)
		{
			Renderer2D::BeginScene(viewProjection);
			for (auto& dc : s_Data.DrawList)
				Renderer::DrawAABB(dc.Mesh, dc.Transform);
			Renderer2D::EndScene();
		}

		Renderer::EndRenderPass();
	}

	void SceneRenderer::CompositePass()
	{

		Renderer::BeginRenderPass(s_Data.CompositePass);
		s_Data.CompositeShader->Bind();
		s_Data.CompositeShader->SetFloat("u_Exposure", s_Data.SceneData.Exposure);
		s_Data.CompositeShader->SetInt("u_TextureSamples", s_Data.GeoPass->GetSpecs().TargetFrameBuffer->GetSpecs().Samples);

		s_Data.GeoPass->GetSpecs().TargetFrameBuffer->BindAsTexture();
		Renderer::SubmitFullscreenQuad(nullptr);
		Renderer::EndRenderPass();
	}

	Ref<RenderPass> SceneRenderer::GetFinalRenderPass()
	{
		return s_Data.CompositePass;
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