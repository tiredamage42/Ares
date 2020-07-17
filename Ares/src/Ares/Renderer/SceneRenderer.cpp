#include "SceneRenderer.h"

namespace Ares
{
	void SceneRenderer::EndScene()
	{
		ARES_CORE_ASSERT(s_Data.ActiveScene, "");
		s_Data.ActiveScene = nullptr;
		FlushDrawList();
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

		Ref<TextureCube> envUnfiltered = TextureCube::Create(TextureFormat::Float16, cubemapSize, cubemapSize);
		Ref<Shader> equirectangularConversionShader = Shader::Create("Assets/Shaders/EquirectangularToCubeMap.glsl");
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
