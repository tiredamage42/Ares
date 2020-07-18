#pragma once

#include "Ares/Scene/Scene.h"
#include "Ares/Renderer/RenderPass.h"

namespace Ares
{
	class SceneRenderer
	{
	public:
		static void Init();
		static void SetViewportSize(uint32_t width, uint32_t height);
		
		static void BeginScene(const Scene* scene);
		static void EndScene();
		
		static void SubmitEntity(Entity* entity);
		
		static Ref<TextureCube> CreateEnvironmentMap(const std::string& filepath);

		static Ref<Texture2D> GetFinalColorBuffer();

		// temp
		static uint32_t GetFinalColorBufferRendererID();

	private:
		static void FlushDrawList();
		static void GeometryPass();
		static void CompositePass();

	};

}