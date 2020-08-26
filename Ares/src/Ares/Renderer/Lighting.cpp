#include "AresPCH.h"
#include "Lighting.h"
#include <glad/glad.h>
#include "Renderer.h"
namespace Ares
{

	struct Data
	{

		// filtered cubemap of skybox reflection
		Ref<TextureCube> ReflectionsTexture;

		// used for PBR lighting
		Ref<TextureCube> IrradianceTexture;
		Ref<Texture2D> BRDF_LUT;

		// used for capturing teh current skybox, to use for irradiance and filtered reflections textures
		Ref<TextureCube> SkyboxCaptureTexture;

		uint32_t SkyboxCaptureFBO;
	};
	static Data s_Data;


	const uint32_t CAPTURE_SKYBOX_SIZE = 2048;
	/*
		to capture the current skybox onto the cubemap faces. set up 6 different view matrices (facing each side of the cube),
		set up a projection matrix with a fov of 90 degrees to capture the entire face, and render the skybox 6 times storing the 
		results in a floating point framebuffer:
	*/
	const Matrix4 CAPTURE_SKYBOX_PROJECTION = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	const Matrix4 CAPTURE_SKYBOX_MATRICES[] =
	{
	   glm::inverse(CAPTURE_SKYBOX_PROJECTION * glm::lookAt(Vector3(0, 0, 0), Vector3( 1.0f,  0.0f,  0.0f), Vector3(0, -1.0f,  0.0f))),
	   glm::inverse(CAPTURE_SKYBOX_PROJECTION * glm::lookAt(Vector3(0, 0, 0), Vector3(-1.0f,  0.0f,  0.0f), Vector3(0, -1.0f,  0.0f))),
	   glm::inverse(CAPTURE_SKYBOX_PROJECTION * glm::lookAt(Vector3(0, 0, 0), Vector3( 0.0f,  1.0f,  0.0f), Vector3(0,  0.0f,  1.0f))),
	   glm::inverse(CAPTURE_SKYBOX_PROJECTION * glm::lookAt(Vector3(0, 0, 0), Vector3( 0.0f, -1.0f,  0.0f), Vector3(0,  0.0f, -1.0f))),
	   glm::inverse(CAPTURE_SKYBOX_PROJECTION * glm::lookAt(Vector3(0, 0, 0), Vector3( 0.0f,  0.0f,  1.0f), Vector3(0, -1.0f,  0.0f))),
	   glm::inverse(CAPTURE_SKYBOX_PROJECTION * glm::lookAt(Vector3(0, 0, 0), Vector3( 0.0f,  0.0f, -1.0f), Vector3(0, -1.0f,  0.0f)))
	};

	const Ref<TextureCube> Lighting::GetReflectionsCubeMap() 
	{ 
		return s_Data.ReflectionsTexture; 
	}
	
	void Lighting::Init()
	{
		/*
			construct the BRDF lookup texture for pbr rendering
		*/
		const uint32_t BRDF_SIZE = 512;
		s_Data.BRDF_LUT = Texture2D::Create(TextureFormat::RG16, BRDF_SIZE, BRDF_SIZE, TextureWrap::Clamp, FilterType::Bilinear, false);
		Shader::Find("Assets/Shaders/CreateBRDF.glsl")->Bind(ShaderVariations::Default);
		Renderer::Submit([BRDF_SIZE]() {
			glBindImageTexture(0, s_Data.BRDF_LUT->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RG16F);
			glDispatchCompute(BRDF_SIZE / 32, BRDF_SIZE / 32, 1);
		}, "");

		s_Data.BRDF_LUT->Bind(Renderer::BRDF_LUT_TEX_SLOT);
		
		/*
			construct the frame buffer and cubemap into which we'll capture the skybox
			when updating reflections and image based lighting
		*/
		Renderer::Submit([]() { glGenFramebuffers(1, &s_Data.SkyboxCaptureFBO); }, "");
		s_Data.SkyboxCaptureTexture = TextureCube::Create(TextureFormat::Float16, CAPTURE_SKYBOX_SIZE, CAPTURE_SKYBOX_SIZE, FilterType::Trilinear, true);

	}

	void Lighting::Shutdown()
	{
		Renderer::Submit([]() { glDeleteFramebuffers(1, &s_Data.SkyboxCaptureFBO); }, "");
	}


	void Lighting::CaptureCurrentSkybox(Ref<Material> skyboxMaterial)
	{
		skyboxMaterial->GetShader()->Bind(ShaderVariations::Default);
		skyboxMaterial->Bind();
		Renderer::GetRendererData().m_FullscreenQuadVertexArray->Bind();

		Renderer::Submit([=]() {
			glDepthMask(false);
			glViewport(0, 0, CAPTURE_SKYBOX_SIZE, CAPTURE_SKYBOX_SIZE);
			glBindFramebuffer(GL_FRAMEBUFFER, s_Data.SkyboxCaptureFBO);
			}, "");

		for (uint32_t i = 0; i < 6; ++i)
		{
			Renderer::Submit([=]() {
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, s_Data.SkyboxCaptureTexture->GetRendererID(), 0);
				glClear(GL_COLOR_BUFFER_BIT);
				}, "");

			skyboxMaterial->GetShader()->SetMat4("ares_InverseVP", CAPTURE_SKYBOX_MATRICES[i]);
			Renderer::DrawIndexed(6, PrimitiveType::Triangles, false);
		}

		Renderer::Submit([]() {
			glGenerateTextureMipmap(s_Data.SkyboxCaptureTexture->GetRendererID());
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDepthMask(true);
			}, "");
	}

	
	/*
		creates a reflection cubemap (where mip levels are filtered for roughness)
		and an irradiance cube (for pbr lighting)
	*/
	void Lighting::UpdateGI(Ref<Material> skyboxMaterial)
	{
		const uint32_t irradianceMapSize = 32;
		CaptureCurrentSkybox(skyboxMaterial);

		s_Data.ReflectionsTexture = TextureCube::Create(TextureFormat::Float16, CAPTURE_SKYBOX_SIZE, CAPTURE_SKYBOX_SIZE, FilterType::Trilinear, true);

		// copy mip map 0 to filtered (need unfiltered as input texture)
		// maybe just eq2cube sample in filter step?
		Renderer::Submit([]() {

			glCopyImageSubData(
				s_Data.SkyboxCaptureTexture->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
				s_Data.ReflectionsTexture->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
				s_Data.ReflectionsTexture->GetWidth(),
				s_Data.ReflectionsTexture->GetHeight(), 6
			);
			}, "");

		Ref<Shader> envFilteringShader = Shader::Find("Assets/Shaders/EnvironmentMipFilter.glsl");
		envFilteringShader->Bind(ShaderVariations::Default);
		s_Data.SkyboxCaptureTexture->Bind();

		int32_t mipCount = s_Data.ReflectionsTexture->GetMipLevelCount();

		Renderer::Submit([mipCount, envFilteringShader]() {
			const float deltaRoughness = 1.0f / glm::max((float)(mipCount - 1.0f), 1.0f);
			for (int32_t mipLevel = 1, size = CAPTURE_SKYBOX_SIZE / 2; mipLevel < mipCount; mipLevel++, size /= 2) // <= ?
			{
				glBindImageTexture(0, s_Data.ReflectionsTexture->GetRendererID(), mipLevel, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
				// set the first float uniform to the roughness factor
				glProgramUniform1f(envFilteringShader->GetRendererID(ShaderVariations::Default), 0, mipLevel * deltaRoughness);
				const GLuint numGroups = glm::max(1, size / 32);
				glDispatchCompute(numGroups, numGroups, 6);
			}
			}, "");

		Ref<Shader> envIrradianceShader = Shader::Find("Assets/Shaders/EnvironmentIrradiance.glsl");

		s_Data.IrradianceTexture = TextureCube::Create(TextureFormat::Float16, irradianceMapSize, irradianceMapSize, FilterType::Trilinear, false);
		envIrradianceShader->Bind(ShaderVariations::Default);

		s_Data.ReflectionsTexture->Bind();

		Renderer::Submit([irradianceMapSize]() {
			glBindImageTexture(0, s_Data.IrradianceTexture->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
			glDispatchCompute(irradianceMapSize / 32, irradianceMapSize / 32, 6);
			//glGenerateTextureMipmap(irradianceMap->GetRendererID());
			}, "");


		s_Data.ReflectionsTexture->Bind(Renderer::ENVIRONMENT_CUBE_TEX_SLOT);
		s_Data.IrradianceTexture->Bind(Renderer::ENVIRONMENT_IRRADIANCE_TEX_SLOT);

		
	}

}
