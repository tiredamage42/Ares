#include "AresPCH.h"

#include "SceneRenderer.h"

#include "Renderer.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Ares
{
	struct SceneRendererData
	{
		const Scene* ActiveScene = nullptr;
		struct SceneInfo
		{
			Camera SceneCamera;

			// Resources
			Ref<MaterialInstance> SkyboxMaterial;
			Ref<TextureCube> EnvironmentRadianceMap;
			Ref<TextureCube> EnvironmentIrradianceMap;


		} SceneData;

		Ref<Texture2D> BRDFLUT;
		Ref<Shader> CompositeShader;

		Ref<RenderPass> GeoPass;
		Ref<RenderPass> CompositePass;

		struct DrawCommand
		{
			Ref<Mesh> Mesh;
			Ref<MaterialInstance> Material;
			glm::mat4 Transform;
		};

		std::vector<DrawCommand> DrawList;

		//grid
		Ref<MaterialInstance> GridMaterial;
	};

	static SceneRendererData s_Data;

	void SceneRenderer::Init()
	{
		FrameBufferSpecs geoFrameBufferSpecs;
		geoFrameBufferSpecs.Width = 1280;
		geoFrameBufferSpecs.Height = 720;
		geoFrameBufferSpecs.Format = FramebufferFormat::RGBA16F;
		geoFrameBufferSpecs.ClearColor = { .1f, .1f, .1f, 1.0f };

		RenderPassSpecs geoRenderPassSpecs;
		geoRenderPassSpecs.TargetFrameBuffer = FrameBuffer::Create(geoFrameBufferSpecs);
		s_Data.GeoPass = RenderPass::Create(geoRenderPassSpecs);



		FrameBufferSpecs compFrameBufferSpecs;
		compFrameBufferSpecs.Width = 1280;
		compFrameBufferSpecs.Height = 720;
		compFrameBufferSpecs.Format = FramebufferFormat::RGBA8;
		compFrameBufferSpecs.ClearColor = { .5f, .1f, .1f, 1.0f };

		RenderPassSpecs compRenderPassSpecs;
		compRenderPassSpecs.TargetFrameBuffer = FrameBuffer::Create(compFrameBufferSpecs);
		s_Data.CompositePass = RenderPass::Create(compRenderPassSpecs);


		s_Data.CompositeShader = Shader::Create("Assets/Shaders/hdr.glsl");
		s_Data.BRDFLUT = Texture2D::Create("Assets/Textures/BRDF_LUT.tga");

		// grid
		auto gridShader = Shader::Create("Assets/Shaders/Grid.glsl");
		s_Data.GridMaterial = MaterialInstance::Create(Material::Create(gridShader));
		float gridScale = 16.025f, gridSize = .025f;
		s_Data.GridMaterial->Set("u_Scale", gridScale);
		s_Data.GridMaterial->Set("u_Res", gridSize);
	}

	void SceneRenderer::SetViewportSize(uint32_t width, uint32_t height)
	{
		s_Data.GeoPass->GetSpecs().TargetFrameBuffer->Resize(width, height);
		s_Data.CompositePass->GetSpecs().TargetFrameBuffer->Resize(width, height);
	}

	void SceneRenderer::BeginScene(const Scene* scene)
	{
		ARES_CORE_ASSERT(!s_Data.ActiveScene, "");
		s_Data.ActiveScene = scene;

		s_Data.SceneData.SceneCamera = scene->m_Camera;
		s_Data.SceneData.SkyboxMaterial = scene->m_SkyboxMaterial;
		s_Data.SceneData.EnvironmentRadianceMap = scene->m_EnvironmentRadianceMap;
		s_Data.SceneData.EnvironmentIrradianceMap = scene->m_EnvironmentIrradianceMap;
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
		s_Data.SceneData = {};
	}

	Ref<Texture2D> SceneRenderer::GetFinalColorBuffer()
	{
		// return s_Data.CompositePass->GetSpecs().TargetFrameBuffer
		ARES_CORE_ASSERT(false, "Not implemented");
		return nullptr;
	}

	uint32_t SceneRenderer::GetFinalColorBufferRendererID()
	{
		return s_Data.CompositePass->GetSpecs().TargetFrameBuffer->GetColorAttachmentRendererID();
	}

	void SceneRenderer::GeometryPass()
	{
		Renderer::BeginRenderPass(s_Data.GeoPass);
		auto viewProjection = s_Data.SceneData.SceneCamera.GetProjectionMatrix() * s_Data.SceneData.SceneCamera.GetViewMatrix();

		// skybox
		auto skyboxShader = s_Data.SceneData.SkyboxMaterial->GetShader();
		s_Data.SceneData.SkyboxMaterial->Set("u_InverseVP", glm::inverse(viewProjection));
		//s_Data.SceneInfo.EnvironmentIrradianceMap->Bind(0);
		Renderer::SubmitFullScreenQuad(s_Data.SceneData.SkyboxMaterial);

		// render entities
		for (auto& dc : s_Data.DrawList)
		{
			dc.Material->Set("u_ViewProjectionMatrix", viewProjection);
			dc.Material->Set("u_CameraPosition", s_Data.SceneData.SceneCamera.GetPosition());

			// environment (todo: dont do this per mesh)
			dc.Material->Set("u_EnvRadianceTex", s_Data.SceneData.EnvironmentRadianceMap);
			dc.Material->Set("u_EnvIrradianceTex", s_Data.SceneData.EnvironmentIrradianceMap);
			dc.Material->Set("u_BRDFLUTTexture", s_Data.BRDFLUT);

			Renderer::SubmitMesh(dc.Mesh, dc.Transform, dc.Material);
		}

		// grid
		s_Data.GridMaterial->Set("u_ViewProjectionMatrix", viewProjection);

		Renderer::SubmitQuad(s_Data.GridMaterial, glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f,0.0f,0.0f)) * glm::scale(glm::mat4(1.0f), ))

		Renderer::EndRenderPass();

	}

	void SceneRenderer::CompositePass()
	{
		Renderer::BeginRenderPass(s_Data.CompositePass);
		s_Data.CompositeShader->Bind();
		s_Data.CompositeShader->SetFloat("u_Exposure", s_Data.SceneData.SceneCamera.GetExposure());
		
		// maybe composite pass???
		s_Data.GeoPass->GetSpecs().TargetFrameBuffer->BindTexture();
		

		Renderer::SubmitFullscreenQuad(nullptr);
		Renderer::EndRenderPass();
	}

	void SceneRenderer::SubmitEntity(Entity* entity)
	{
		// culling, sorting, etc. HERE

		auto mesh = entity->GetMesh();
		if (!mesh)
			return;

		s_Data.DrawList.push_back({
			mesh, 
			entity->GetMaterial(), 
			entity->GetTransform()
		});
	}


	void SceneRenderer::CreateEnvironmentMap(const std::string& filepath)
	{
		const uint32_t cubemapSize = 1024;

		// create blank texture cube
		Ref<TextureCube> envUnfiltered = TextureCube::Create(TextureFormat::Float16, cubemapSize, cubemapSize);
		
		Ref<Shader> equirectangularConversionShader = Shader::Create("Assets/Shaders/EquirectangularToCubeMap.glsl");
		
		// load teh 2d texture we're converting to cubemap
		Ref<Texture2D> envEquirect = Texture2D::Create(filepath);
		
		ARES_CORE_ASSERT(envEquirect->GetFormat() == TextureFormat::Float16, "Texture is not HDR!");

		equirectangularConversionShader->Bind();
		envEquirect->Bind(0);

		Renderer::Submit([=]()
		{
			glBindImageTexture(0, envUnfiltered->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16);
			glDispatchCompute(cubemapSize / 32, cubemapSize / 32, 6);
			glGenerateTextureMipmap(envUnfiltered->GetRendererID());
		});
	}
}
