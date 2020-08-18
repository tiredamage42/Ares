#include "AresPCH.h"
#include "SceneRenderer.h"

#include "Renderer.h"
#include "Ares/Core/Entity.h"
#include "Ares/Core/Components.h"
#include "Ares/Core/Application.h"
#include "Ares/Renderer/Renderer2D.h"
#include <glm/gtc/matrix_transform.hpp>


//#include <GLFW/glfw3.h>
#include <glad/glad.h>

/*
	TODO:
		update gi button/function:
			renders skybox to cube
			creates radiance and irradiance textures from it

		this way we can have procedural skybox


*/
namespace Ares {

	struct ShaderVariationPair
	{
		Ref<Shader> Shader;
		ShaderVariant Variant;

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
//#define GRID_RESOLUTION 2147483646
//#define GRID_RESOLUTION 100
//#define GRID_WIDTH .05f

	struct MeshDrawCall
	{
		Ref<Mesh> Mesh;
		size_t SubmeshIndex;
		glm::mat4 Transform;
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

			// Resources
			//float SkyboxLod, Exposure;
			float Exposure;
			//Ref<MaterialInstance> SkyboxMaterial;
			Ref<Material> SkyboxMaterial;
			Environment SceneEnvironment;
			Light ActiveLight;
		} SceneData;

		Ref<Texture2D> BRDFLUT;
		Ref<Shader> CompositeShader;

		Ref<RenderPass> GeoPass;
		Ref<RenderPass> CompositePass;

		/*
		*/
		struct DrawCommand
		{
			Ref<Mesh> Mesh;
			//Ref<MaterialInstance> MaterialOverride;
			//std::vector<Ref<Material>> Materials;
			glm::mat4 Transform;
			//std::string name;
		};
		std::vector<DrawCommand> DrawList;
		std::vector<DrawCommand> SelectedMeshDrawList;

		RenderMap DrawMap;
		RenderMap SelectedDrawMap;

		// Grid
		//Ref<MaterialInstance> GridMaterial;
		//Ref<MaterialInstance> OutlineMaterial;
		//Ref<Material> GridMaterial;
		Ref<Shader> GridShader;


		//Ref<Material> OutlineMaterial;
		Ref<Shader> OutlineShader;

		SceneRendererOptions Options;
	};

	static SceneRendererData s_Data;


	
	/*
	static void SortRenderMap(const std::vector<SceneRendererData::DrawCommand>& drawList, RenderMap& drawMap)
	{
		ARES_PROFILE_FUNCTION();

		for (auto& dc : drawList)
		{
			Ref<Mesh> mesh = dc.Mesh;
			std::vector<Ref<Material>> materials = dc.Materials;

			ShaderVariant shaderVariant = mesh->IsAnimated() ? ShaderVariant::Skinned : ShaderVariant::Static;

			auto& submeshes = mesh->GetSubmeshes();
			for (size_t i = 0; i < submeshes.size(); i++)
			{
				Ref<Material> material = materials[submeshes[i].MaterialIndex];

				ShaderVariationPair shaderVarPair = { material->GetShader(), shaderVariant };

				if (drawMap.find(shaderVarPair) == drawMap.end())
					drawMap[shaderVarPair] = MaterialsMap();

				MaterialsMap& materialsMap = drawMap.at(shaderVarPair);

				if (materialsMap.find(material) == materialsMap.end())
					materialsMap[material] = MeshDrawCalls();

				MeshDrawCalls& drawCalls = materialsMap.at(material);

				drawCalls.push_back({ mesh, i, dc.Transform });
			}
		}
	}
	*/

	/*
	static const size_t u_EnvRadianceTex = StringUtils::String2Hash("u_EnvRadianceTex");
	static const size_t u_EnvIrradianceTex = StringUtils::String2Hash("u_EnvIrradianceTex");
	static const size_t u_BRDFLUTTexture = StringUtils::String2Hash("u_BRDFLUTTexture");
	*/

	static void DrawRenderMap(const RenderMap& renderMap, const glm::mat4& viewProjection, const glm::mat4& view)//, const glm::vec3& cameraPosition)
	{
		ARES_PROFILE_FUNCTION();

		// Read our shaders into the appropriate buffers
		for (auto& shader_materials : renderMap)
		{
			const ShaderVariationPair& shaderVarPair = shader_materials.first;
			Ref<Shader> shader = shaderVarPair.Shader;
			ShaderVariant variant = shaderVarPair.Variant;
			
			ARES_PROFILE_SCOPE("per shader");

			{
				ARES_PROFILE_SCOPE("bind shader set matrices");
			shader->Bind(variant);
			shader->SetMat4("ares_VPMatrix", viewProjection, variant);
			shader->SetMat4("ares_VMatrix", view, variant);
			}
			

			const MaterialsMap& materials = shader_materials.second;
			for (auto& materials_draws : materials)
			{
				ARES_PROFILE_SCOPE("Per material");
				Ref<Material> material = materials_draws.first;
				const MeshDrawCalls& draws = materials_draws.second;

				//material->Set("u_CameraPosition", cameraPosition);

				{
					ARES_PROFILE_SCOPE("Set MAterial things");
				material->SetTexture("u_EnvRadianceTex", s_Data.SceneData.SceneEnvironment.RadianceMap);
				
				// pbr specific...
				material->SetTexture("u_EnvIrradianceTex", s_Data.SceneData.SceneEnvironment.IrradianceMap);
				material->SetTexture("u_BRDFLUTTexture", s_Data.BRDFLUT);
				}

				{
					ARES_PROFILE_SCOPE("biind material");
					material->Bind(variant);
				}

				bool depthTest = material->GetFlag(MaterialFlag::DepthTest);
				// foreach light (if shader is lit)
				{
					{
						ARES_PROFILE_SCOPE("set shader light vars");
					shader->SetFloat3("ares_Light.Direction", s_Data.SceneData.ActiveLight.Direction, variant);
					shader->SetFloat3("ares_Light.Color", s_Data.SceneData.ActiveLight.Radiance * s_Data.SceneData.ActiveLight.Multiplier, variant);
					}
					
					
					// do shader set light light
					//material->Set("lights", s_Data.SceneData.ActiveLight);

					{
						ARES_PROFILE_SCOPE("Draw Mesh");
						for (auto& dc : draws)
						{
							Renderer::SubmitMesh(shader, dc.Mesh, dc.Transform, dc.SubmeshIndex, depthTest);
						}
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
		
		
		
		
		//s_Data.BRDFLUT = Texture2D::Create("Assets/Textures/BRDF_LUT.tga", FilterType::Point, false);
		const uint32_t BRDF_SIZE = 512;
		s_Data.BRDFLUT = Texture2D::Create(TextureFormat::RG16, BRDF_SIZE, BRDF_SIZE, TextureWrap::Clamp, FilterType::Bilinear, false);

		Ref<Shader> createBRDFLUTShader = Shader::Find("Assets/Shaders/CreateBRDF.glsl");

		createBRDFLUTShader->Bind(ShaderVariant::Static);

		Renderer::Submit([BRDF_SIZE]() {
			glBindImageTexture(0, s_Data.BRDFLUT->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RG16F);
			glDispatchCompute(BRDF_SIZE / 32, BRDF_SIZE / 32, 1);
		}, "EnvMap 0");






		// Grid
		//auto gridShader 
		s_Data.GridShader = Shader::Find("Assets/Shaders/SceneGrid.glsl");

		//s_Data.GridMaterial = CreateRef<MaterialInstance>(CreateRef<Material>(gridShader));
		//s_Data.GridMaterial = CreateRef<Material>(gridShader);

		// TODO: SET SCENE VALUES
		/*float gridScale = 16.025f, gridSize = 0.025f;
		s_Data.GridMaterial->Set("u_Scale", gridScale);
		s_Data.GridMaterial->Set("u_Res", gridSize);*/

		//s_Data.GridMaterial->Set("u_MVP", viewProjection * glm::scale(glm::mat4(1.0f), glm::vec3(m_GridScale - m_GridSize)));
		


		//s_Data.GridMaterial->SetValue("u_ResAndWidth", glm::vec2(GRID_RESOLUTION, GRID_WIDTH));

		//s_Data.GridMaterial->SetValue("u_Scale", (float)GRID_RESOLUTION);
		//s_Data.GridMaterial->SetValue("u_Res", GRID_WIDTH);


		// Outline
		auto outlineShader = Shader::Find("Assets/Shaders/UnlitColor.glsl");
		s_Data.OutlineShader = outlineShader;

		//s_Data.OutlineMaterial = CreateRef<MaterialInstance>(CreateRef<Material>(outlineShader));
		//s_Data.OutlineMaterial = CreateRef<Material>(outlineShader);
		//s_Data.OutlineMaterial->SetFlag(MaterialFlag::DepthTest, false);
		

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
		//s_Data.SceneData.SkyboxLod = scene->m_SkyboxLod;
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

		s_Data.DrawMap.clear();
		s_Data.SelectedDrawMap.clear();

		s_Data.DrawList.clear();
		s_Data.SelectedMeshDrawList.clear();

		s_Data.SceneData = {};
	}

	void SceneRenderer::SubmitMesh(Ref<Mesh> mesh, const glm::mat4& transform, std::vector<Ref<Material>> materials, bool isSelected)
	{
		
		ShaderVariant shaderVariant = mesh->IsAnimated() ? ShaderVariant::Skinned : ShaderVariant::Static;

		RenderMap& drawMap = isSelected ? s_Data.SelectedDrawMap : s_Data.DrawMap;

		std::vector<SceneRendererData::DrawCommand> drawList = isSelected ? s_Data.SelectedMeshDrawList : s_Data.DrawList;
		drawList.push_back({ mesh, transform });



		auto& submeshes = mesh->GetSubmeshes();
		for (size_t i = 0; i < submeshes.size(); i++)
		{
			Ref<Material> material = materials[submeshes[i].MaterialIndex];

			ShaderVariationPair shaderVarPair = { material->GetShader(), shaderVariant };

			if (drawMap.find(shaderVarPair) == drawMap.end())
				drawMap[shaderVarPair] = MaterialsMap();

			MaterialsMap& materialsMap = drawMap.at(shaderVarPair);

			if (materialsMap.find(material) == materialsMap.end())
				materialsMap[material] = MeshDrawCalls();

			MeshDrawCalls& drawCalls = materialsMap.at(material);

			drawCalls.push_back({ mesh, i, transform });
		}
		
	}
	/*
	//void SceneRenderer::SubmitMesh(Ref<Mesh> mesh, const glm::mat4& transform, Ref<MaterialInstance> materialOverride)
	void SceneRenderer::SubmitMesh(Ref<Mesh> mesh, const glm::mat4& transform, std::vector<Ref<Material>> materials, const std::string& name)

	//void SceneRenderer::SubmitEntity(Entity& entity)
	{
		// TODO: Culling, sorting, etc.

		/auto& mrComponent = entity.GetComponent<MeshRendererComponent>();
		if (!mrComponent.Mesh)
			return;/
		
		s_Data.DrawList.push_back({ mesh, materials, transform, name });
	}
	//void SceneRenderer::SubmitSelectedMesh(Ref<Mesh> mesh, const glm::mat4& transform, Ref<MaterialInstance> materialOverride)
	void SceneRenderer::SubmitSelectedMesh(Ref<Mesh> mesh, const glm::mat4& transform, std::vector<Ref<Material>> materials)
	{
		s_Data.SelectedMeshDrawList.push_back({ mesh, materials, transform, "selected" });
	}
	*/

	//static Ref<Shader> equirectangularConversionShader, envFilteringShader, envIrradianceShader;

	//std::pair<Ref<TextureCube>, Ref<TextureCube>> SceneRenderer::CreateEnvironmentMap(const std::string& filepath)
	//{
	//	const uint32_t CAPTURE_SIZE = 512;
	//	const uint32_t IRRADIANCE_SIZE = 32;

	//	// pbr: setup framebuffer
	//	uint32_t captureFBO;
	//	uint32_t captureRBO;

	//	glGenFramebuffers(1, &captureFBO);
	//	glGenRenderbuffers(1, &captureRBO);

	//	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	//	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, CAPTURE_SIZE, CAPTURE_SIZE);
	//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	//	// pbr: load the HDR environment map
	//	// ---------------------------------
	//	Ref<Texture2D> hdrTex = Texture2D::Create(filepath, FilterType::Trilinear, false);

	//	// pbr: setup cubemap to render to and attach to framebuffer
	//	// ---------------------------------------------------------
	//	Ref<TextureCube> envCube = TextureCube::Create(TextureFormat::Float16, CAPTURE_SIZE, CAPTURE_SIZE, FilterType::Bilinear, false);

	//	// pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
	//	// ----------------------------------------------------------------------------------------------
	//	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	//	glm::mat4 captureViews[] =
	//	{
	//		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	//		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	//		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	//		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	//		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	//		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	//	};

	//	// pbr: convert HDR equirectangular environment map to cubemap equivalent
	//	// ----------------------------------------------------------------------

	//	Ref<Shader> eq2CubeShader = Shader::Find("Assets/Shaders/Eq2Cube.glsl");
	//	eq2CubeShader->Bind();
	//	eq2CubeShader->SetInt("u_Texture", 0);
	//	eq2CubeShader->SetMat4("u_Projection", captureProjection);
	//	hdrTex->Bind();
	//	
	//	Renderer::Submit([captureViews, eq2CubeShader]() {
	//	
	//		// don't forget to configure the viewport to the capture dimensions.
	//		glViewport(0, 0, CAPTURE_SIZE, CAPTURE_SIZE); 
	//	
	//		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	//	
	//		for (uint32_t i = 0; i < 6; ++i)
	//		{
	//			 
	//			eq2CubeShader->SetMat4FromRenderThread("u_View", captureViews[i]);
	//		
	//			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCube->GetRendererID(), 0);
	//		
	//			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//			renderCube();
	//		}
	//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//	}, "capture env map");

	//	// pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
	//	// --------------------------------------------------------------------------------
	//	Ref<TextureCube> irradianceMap = TextureCube::Create(TextureFormat::Float16, IRRADIANCE_SIZE, IRRADIANCE_SIZE, FilterType::Bilinear, false);

	//	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	//	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, IRRADIANCE_SIZE, IRRADIANCE_SIZE);

	//	// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
	//	// -----------------------------------------------------------------------------

	//	Ref<Shader> irradianceShader = Shader::Find("Assets/Shaders/Irradiance.glsl");

	//	irradianceShader->Bind();
	//	irradianceShader->SetInt("u_Texture", 0);
	//	irradianceShader->SetMat4("u_Projection", captureProjection);

	//	envCube->Bind();
	//	
	//	// don't forget to configure the viewport to the capture dimensions.
	//	glViewport(0, 0, IRRADIANCE_SIZE, IRRADIANCE_SIZE); 
	//	
	//	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	//	
	//	for (unsigned int i = 0; i < 6; ++i)
	//	{
	//		irradianceShader->SetMat4("u_View", captureViews[i]);

	//		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap->GetRendererID(), 0);
	//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//		renderCube();
	//	}
	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//	// then before rendering, configure the viewport to the original framebuffer's screen dimensions
	//	int scrWidth, scrHeight;
	//	glfwGetFramebufferSize((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), &scrWidth, &scrHeight);

	//	glViewport(0, 0, scrWidth, scrHeight);

	//	glDeleteFramebuffers(1, &captureFBO);
	//	glDeleteRenderbuffers(1 & captureRBO);
	//}


	std::pair<Ref<TextureCube>, Ref<TextureCube>> SceneRenderer::CreateEnvironmentMap(const std::string& filepath)
	{

		
		const uint32_t cubemapSize = 2048;
		//const uint32_t irradianceMapSize = 2048;
		const uint32_t irradianceMapSize = 32;

		// env unfiltered
		Ref<TextureCube> envUnfiltered = TextureCube::Create(TextureFormat::Float16, cubemapSize, cubemapSize, FilterType::Trilinear, true);
		
		//if (!equirectangularConversionShader)
		Ref<Shader> equirectangularConversionShader = Shader::Find("Assets/Shaders/EquirectangularToCubeMap.glsl");
		
		Ref<Texture2D> envEquirect = Texture2D::Create(filepath, FilterType::Trilinear, false);

		ARES_CORE_ASSERT(envEquirect->GetFormat() == TextureFormat::Float16, "Texture is not HDR!");
		 
		equirectangularConversionShader->Bind(ShaderVariant::Static);
		envEquirect->Bind();
		Renderer::Submit([envUnfiltered, cubemapSize, envEquirect](){

			glBindImageTexture(0, envUnfiltered->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
			glDispatchCompute(cubemapSize / 32, cubemapSize / 32, 6);
			glGenerateTextureMipmap(envUnfiltered->GetRendererID());
		}, "EnvMap 0");
		
		//envUnfiltered->GenerateMipMaps();


		//if (!envFilteringShader)
		Ref<Shader> envFilteringShader = Shader::Find("Assets/Shaders/EnvironmentMipFilter.glsl");

		Ref<TextureCube> envFiltered = TextureCube::Create(TextureFormat::Float16, cubemapSize, cubemapSize, FilterType::Trilinear, true);

		// copy mip map 0 to filtered (need unfiltered as input texture)
		// maybe just eq2cube sample in filter step?
		Renderer::Submit([envUnfiltered, envFiltered](){

			glCopyImageSubData(
				envUnfiltered->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
				envFiltered->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
				envFiltered->GetWidth(), envFiltered->GetHeight(), 6
			);
		}, "EnvMap 1");


		envFilteringShader->Bind(ShaderVariant::Static);
		envUnfiltered->Bind();

		uint32_t mipCount = envFiltered->GetMipLevelCount();

		Renderer::Submit([envUnfiltered, envFiltered, cubemapSize, mipCount, envFilteringShader]() {
			
			const float deltaRoughness = 1.0f / glm::max((float)(mipCount - 1.0f), 1.0f);

			for (int mipLevel = 1, size = cubemapSize / 2; mipLevel < mipCount; mipLevel++, size /= 2) // <= ?
			{
				glBindImageTexture(0, envFiltered->GetRendererID(), mipLevel, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
				
				//float roughness = (float)mipLevel / (float)(mipCount - 1);
				// set the first float uniform to the roughness factor
				glProgramUniform1f(envFilteringShader->GetRendererID(ShaderVariant::Static), 0, mipLevel * deltaRoughness);
				//glProgramUniform1f(envFilteringShader->GetRendererID(), 0, roughness);

				const GLuint numGroups = glm::max(1, size / 32);
				glDispatchCompute(numGroups, numGroups, 6);
			}
		}, "EnvMap 2");

		//if (!envIrradianceShader)
		Ref<Shader> envIrradianceShader = Shader::Find("Assets/Shaders/EnvironmentIrradiance.glsl");

		Ref<TextureCube> irradianceMap = TextureCube::Create(TextureFormat::Float16, irradianceMapSize, irradianceMapSize, FilterType::Trilinear, false);
		envIrradianceShader->Bind(ShaderVariant::Static);

		envFiltered->Bind();
		//envCube->Bind();

		Renderer::Submit([irradianceMap, irradianceMapSize](){
				glBindImageTexture(0, irradianceMap->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
				glDispatchCompute(irradianceMapSize / 32, irradianceMapSize / 32, 6);
				//glGenerateTextureMipmap(irradianceMap->GetRendererID());
			}, "EnvMap 3");

		//irradianceMap->GenerateMipMaps();

		return { envFiltered, irradianceMap };
		//return { envCube, irradianceMap };
	}

	void SceneRenderer::GeometryPass()
	{
		ARES_PROFILE_FUNCTION();

		bool outline = false;// s_Data.SelectedMeshDrawList.size() > 0;

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


		auto viewMatrix = s_Data.SceneData.SceneCamera.ViewMatrix;
		auto viewProjection = s_Data.SceneData.SceneCamera.Camera.GetProjectionMatrix() * viewMatrix;
		//glm::vec3 cameraPosition = glm::inverse(s_Data.SceneData.SceneCamera.ViewMatrix)[3];



		// Skybox
		// TODO: render skybox (render as last to prevent overdraw)
		auto skyboxShader = s_Data.SceneData.SkyboxMaterial->GetShader();
		skyboxShader->Bind(ShaderVariant::Static);

		s_Data.SceneData.SkyboxMaterial->SetValue("u_InverseVP", glm::inverse(viewProjection));
		//float skyboxLod = s_Data.ActiveScene->GetSkyboxLod();
		
		//s_Data.SceneData.SkyboxMaterial->Set("u_TextureLod", s_Data.SceneData.SkyboxLod);
		// s_Data.SceneInfo.EnvironmentIrradianceMap->Bind(0);
		Renderer::SubmitFullscreenQuad(s_Data.SceneData.SkyboxMaterial);



		/*
			set standard mesh drawing shader variables
		*/


		//RenderMap drawMap;
		//SortRenderMap(s_Data.DrawList, drawMap);
		//DrawRenderMap(drawMap, viewProjection, viewMatrix);// , cameraPosition);
		DrawRenderMap(s_Data.DrawMap, viewProjection, viewMatrix);// , cameraPosition);
		//drawMap.clear();


		// TODO: render order based on material....
		// Render entities
		
		//std::unordered_set<std::string> shadersVisited;

		//for (auto& dc : s_Data.DrawList)
		//{
		//	//auto baseMaterial = dc.Mesh->GetMaterial();


		//	ShaderVariant variant = dc.Mesh->IsAnimated() ? ShaderVariant::Skinned : ShaderVariant::Static;

		//	const std::vector<Ref<Material>>& materials = dc.Materials;

		//	for (auto material : materials)
		//	//for (size_t i = 0; i < materials.size(); i++)
		//	{

		//		Ref<Shader> shader = material->GetShader();
		//		std::string key = shader->GetName() + std::to_string((size_t)variant);

		//		if (!shadersVisited.count(key))
		//		{

		//			shader->Bind(variant);
		//			shader->SetMat4("ares_VPMatrix", viewProjection, variant);
		//			// maybe set view and projection seperate as well

		//			shadersVisited.insert(key);
		//		}

		//		//baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
		//		material->Set("u_CameraPosition", cameraPosition);

		//		// Environment (TODO: don't do this per mesh)
		//		material->Set("u_EnvRadianceTex", s_Data.SceneData.SceneEnvironment.RadianceMap);
		//		material->Set("u_EnvIrradianceTex", s_Data.SceneData.SceneEnvironment.IrradianceMap);
		//		material->Set("u_BRDFLUTTexture", s_Data.BRDFLUT);

		//		// Set lights (TODO: move to light environment and don't do per mesh)
		//		material->Set("lights", s_Data.SceneData.ActiveLight);

		//	}


		//	/*
		//	Ref<Material> baseMaterial = nullptr;
		//	//if (dc.MaterialOverride)
		//	if (dc.Material)
		//	{

		//		//baseMaterial = dc.MaterialOverride->BaseMaterial();
		//		baseMaterial = dc.Material;
		//	}
		//	else
		//	{
		//		baseMaterial = dc.Mesh->GetMaterial();
		//	}

		//	Ref<Shader> shader = baseMaterial->GetShader();
		//	ShaderVariant variant = dc.Mesh->IsAnimated() ? ShaderVariant::Skinned : ShaderVariant::Static;

		//	std::string key = shader->GetName() + std::to_string((size_t)variant);

		//	if (!shadersVisited.count(key))
		//	{

		//		shader->Bind(variant);
		//		shader->SetMat4("ares_VPMatrix", viewProjection, variant);
		//		// maybe set view and projection seperate as well

		//		shadersVisited.insert(key);
		//	}
		//	*/



		////	auto baseMaterial = dc.Material;// Mesh->GetMaterial();

		//	// make list of shaders from draw list, set this per shader:
		//	{
		//		//ares_VPMatrix

		//		//baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
		//		// maybe set view and projection seperate as well
		//	}

		//	
		//	/*
		//	baseMaterial->Set("u_CameraPosition", cameraPosition);

		//	// Environment (TODO: don't do this per mesh)
		//	baseMaterial->Set("u_EnvRadianceTex", s_Data.SceneData.SceneEnvironment.RadianceMap);
		//	baseMaterial->Set("u_EnvIrradianceTex", s_Data.SceneData.SceneEnvironment.IrradianceMap);
		//	baseMaterial->Set("u_BRDFLUTTexture", s_Data.BRDFLUT);

		//	//ARES_CORE_LOG("LGIHT {0}", s_Data.SceneData.ActiveLight.Multiplier);
		//	// Set lights (TODO: move to light environment and don't do per mesh)
		//	baseMaterial->Set("lights", s_Data.SceneData.ActiveLight);
		//	*/
		//	//auto overrideMaterial = nullptr; // dc.Material;
		//	
		//	Renderer::SubmitMesh(dc.Mesh, dc.Transform, materials);
		//	//Renderer::SubmitMesh(dc.Mesh, dc.Transform, dc.MaterialOverride);

		////	//auto overrideMaterial = nullptr; // dc.Material;

		////	//baseMaterial->Bind();
		//	//Renderer::SubmitMesh(dc.Mesh, dc.Transform);// , baseMaterial->GetShader());
		////	//Renderer::SubmitMesh(dc.Mesh, dc.Transform, overrideMaterial);
		//}



		if (outline)
		{

			
			Renderer::Submit([](){
					glStencilFunc(GL_ALWAYS, 1, 0xff);
					glStencilMask(0xff);
				}, "outline other stuff");
		}



		//SortRenderMap(s_Data.SelectedMeshDrawList, drawMap);
		//DrawRenderMap(drawMap, viewProjection, viewMatrix);// , cameraPosition);
		DrawRenderMap(s_Data.SelectedDrawMap, viewProjection, viewMatrix);// , cameraPosition);


		//shadersVisited.clear();

		//for (auto& dc : s_Data.SelectedMeshDrawList)
		//{
		//	//Ref<Material> baseMaterial = nullptr;
		//	//if (dc.MaterialOverride)
		//	//if (dc.Material)
		//	//{

		//	//	//baseMaterial = dc.MaterialOverride->BaseMaterial();
		//	//	baseMaterial = dc.Material;
		//	//}
		//	//else
		//	//{
		//	//	baseMaterial = dc.Mesh->GetMaterial();
		//	//}

		//	ShaderVariant variant = dc.Mesh->IsAnimated() ? ShaderVariant::Skinned : ShaderVariant::Static;
		//	
		//	for (size_t i = 0; i < dc.Materials.size(); i++)
		//	{
		//		Ref<Shader> shader = dc.Materials[i]->GetShader();
		//		std::string key = shader->GetName() + std::to_string((size_t)variant);

		//		if (!shadersVisited.count(key))
		//		{
		//			shader->Bind(variant);
		//			shader->SetMat4("ares_VPMatrix", viewProjection, variant);
		//			// maybe set view and projection seperate as well
		//			shadersVisited.insert(key);
		//		}

		//		//baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
		//		dc.Materials[i]->Set("u_CameraPosition", cameraPosition);

		//		// Environment (TODO: don't do this per mesh)
		//		dc.Materials[i]->Set("u_EnvRadianceTex", s_Data.SceneData.SceneEnvironment.RadianceMap);
		//		dc.Materials[i]->Set("u_EnvIrradianceTex", s_Data.SceneData.SceneEnvironment.IrradianceMap);
		//		dc.Materials[i]->Set("u_BRDFLUTTexture", s_Data.BRDFLUT);

		//		// Set lights (TODO: move to light environment and don't do per mesh)
		//		dc.Materials[i]->Set("lights", s_Data.SceneData.ActiveLight);

		//	}




		//	////baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
		//	//baseMaterial->Set("u_CameraPosition", cameraPosition);

		//	//// Environment (TODO: don't do this per mesh)
		//	//baseMaterial->Set("u_EnvRadianceTex", s_Data.SceneData.SceneEnvironment.RadianceMap);
		//	//baseMaterial->Set("u_EnvIrradianceTex", s_Data.SceneData.SceneEnvironment.IrradianceMap);
		//	//baseMaterial->Set("u_BRDFLUTTexture", s_Data.BRDFLUT);

		//	//// Set lights (TODO: move to light environment and don't do per mesh)
		//	//baseMaterial->Set("lights", s_Data.SceneData.ActiveLight);

		//	//auto overrideMaterial = nullptr; // dc.Material;

		//	//Renderer::SubmitMesh(dc.Mesh, dc.Transform, dc.MaterialOverride);
		//	Renderer::SubmitMesh(dc.Mesh, dc.Transform, dc.Materials);
		//}

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

			

				//Ref<Shader> outlineShader = s_Data.OutlineMaterial->GetShader();
			
				//std::unordered_set<ShaderVariant> outlineVariantsVisited;
				//std::vector<Ref<Material>> outlineMaterials = { s_Data.OutlineMaterial };

				size_t outlineDrawCount = s_Data.SelectedMeshDrawList.size();
				std::vector<SceneRendererData::DrawCommand> outlineDraws;
				outlineDraws.resize(s_Data.SelectedMeshDrawList.size());
				size_t x = 0;
				size_t y = outlineDrawCount - 1;
				for (auto& dc : s_Data.SelectedMeshDrawList)
				{
					if (dc.Mesh->IsAnimated())
						outlineDraws[y--] = dc;
					/*{
					}*/
					else
						outlineDraws[x++] = dc;
					/*{
					}*/
				}
				size_t skinnedVariationStart = x;

				s_Data.OutlineShader->Bind(ShaderVariant::Static);
				s_Data.OutlineShader->SetMat4("ares_VPMatrix", viewProjection, ShaderVariant::Static);
				s_Data.OutlineShader->SetFloat4("u_Color", glm::vec4(1, .5f, 0, 1), ShaderVariant::Static);

				for (size_t i = 0; i < outlineDrawCount; i++)
				{
					if (i == skinnedVariationStart)
					{
						s_Data.OutlineShader->Bind(ShaderVariant::Skinned);
						s_Data.OutlineShader->SetMat4("ares_VPMatrix", viewProjection, ShaderVariant::Skinned);
						s_Data.OutlineShader->SetFloat4("u_Color", glm::vec4(1, .5f, 0, 1), ShaderVariant::Skinned);
					}

					Renderer::SubmitMesh(s_Data.OutlineShader, outlineDraws[i].Mesh, outlineDraws[i].Transform, false);
				}



				//s_Data.OutlineMaterial->Set("u_ViewProjection", viewProjection);
				//for (auto& dc : s_Data.SelectedMeshDrawList)
				//{
				//	ShaderVariant variant = dc.Mesh->IsAnimated() ? ShaderVariant::Skinned : ShaderVariant::Static;
				//	if (!outlineVariantsVisited.count(variant))
				//	{
				//		s_Data.OutlineMaterial->GetShader()->Bind(variant);
				//		s_Data.OutlineMaterial->GetShader()->SetMat4("ares_VPMatrix", viewProjection, variant);
				//		outlineVariantsVisited.insert(variant);
				//	}
				//
				//	//Renderer::SubmitMesh(dc.Mesh, dc.Transform, s_Data.OutlineMaterial);
				//	Renderer::SubmitMesh(dc.Mesh, dc.Transform, outlineMaterials);
				//}

			





			Renderer::Submit([](){
					glPointSize(10);
					glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				}, "Draw Outline polymode");



			s_Data.OutlineShader->Bind(ShaderVariant::Static);
			
			for (size_t i = 0; i < outlineDrawCount; i++)
			{
				if (i == skinnedVariationStart)
				{
					s_Data.OutlineShader->Bind(ShaderVariant::Skinned);
				}

				Renderer::SubmitMesh(s_Data.OutlineShader, outlineDraws[i].Mesh, outlineDraws[i].Transform, false);
			}

			//for (auto& dc : s_Data.SelectedMeshDrawList)
			//{
			//	//Renderer::SubmitMesh(dc.Mesh, dc.Transform, s_Data.OutlineMaterial);
			//	Renderer::SubmitMesh(dc.Mesh, dc.Transform, outlineMaterials);
			//}

			Renderer::Submit([](){
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glStencilMask(0xff);
					glStencilFunc(GL_ALWAYS, 1, 0xff);
					glEnable(GL_DEPTH_TEST);
				}, "Outline End");
		}





		// DEBUG THE BRDF TEXTURE


		//Ref<Shader> texShader = Shader::Find("Assets/Shaders/Textured.glsl");
		//texShader->Bind();
		//texShader->SetInt("u_Texture", 0);
		//texShader->SetMat4("u_ViewProjection", viewProjection);
		//s_Data.BRDFLUT->Bind();

		////Renderer::SubmitQuad(texShader, glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec3(0, 0, -2)), true);
		//Renderer::SubmitQuad(texShader, glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -2)), true);











		// Grid
		if (GetOptions().ShowGrid)
		{


			//int m_GridScale = 8;
			//float m_GridSize = 0.025f;
			//float m_GridSize = 0.5f;

			s_Data.GridShader->Bind(ShaderVariant::Static);
			s_Data.GridShader->SetMat4("ares_VPMatrix", viewProjection, ShaderVariant::Static);

			/*s_Data.GridMaterial->GetShader()->Bind(ShaderVariant::Static);
			s_Data.GridMaterial->GetShader()->SetMat4("ares_VPMatrix", viewProjection, ShaderVariant::Static);*/

			//s_Data.GridMaterial->Set("ares_VPMatrix", viewProjection);


			//s_Data.GridMaterial->Set("u_MVP", viewProjection * glm::scale(glm::mat4(1.0f), glm::vec3(m_GridScale - m_GridSize)));

			// TODO: get scale from scene params
			//Renderer::SubmitQuad(s_Data.GridMaterial, glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(16.0f)));
			//Renderer::SubmitQuad(s_Data.GridMaterial, glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(m_GridScale - m_GridSize)));
			
			//Renderer::SubmitQuad(s_Data.GridMaterial, glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));// *glm::scale(glm::mat4(1.0f), glm::vec3(GRID_RESOLUTION + GRID_WIDTH)));
			Renderer::SubmitQuad(s_Data.GridShader, glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)), true);// *glm::scale(glm::mat4(1.0f), glm::vec3(GRID_RESOLUTION + GRID_WIDTH)));
		}


		if (GetOptions().ShowBoundingBoxes)
		{
			Renderer2D::BeginScene(viewProjection);
			for (auto& dc : s_Data.DrawList)
				Renderer::DrawAABB(dc.Mesh, dc.Transform);
			Renderer2D::EndScene();
		}

		/*
		// Skybox
		// render skybox (render as last to prevent overdraw)
		//auto skyboxShader = s_Data.SceneData.SkyboxMaterial->GetShader();
		s_Data.SceneData.SkyboxMaterial->Set("u_InverseVP", glm::inverse(viewProjection));
		//float skyboxLod = s_Data.ActiveScene->GetSkyboxLod();
		s_Data.SceneData.SkyboxMaterial->Set("u_TextureLod", s_Data.SceneData.SkyboxLod);
		// s_Data.SceneInfo.EnvironmentIrradianceMap->Bind(0);
		Renderer::SubmitFullscreenQuad(s_Data.SceneData.SkyboxMaterial);
		*/


		Renderer::EndRenderPass();
	}


	void SceneRenderer::CompositePass()
	{
		ARES_PROFILE_FUNCTION();

		Renderer::BeginRenderPass(s_Data.CompositePass);
		s_Data.CompositeShader->Bind(ShaderVariant::Static);
		s_Data.CompositeShader->SetFloat("u_Exposure", s_Data.SceneData.Exposure, ShaderVariant::Static);
		s_Data.CompositeShader->SetInt("u_TextureSamples", s_Data.GeoPass->GetSpecs().TargetFrameBuffer->GetSpecs().Samples, ShaderVariant::Static);

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