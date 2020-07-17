#pragma once

#include "Ares/Scene/Entity.h"
#include "Ares/Scene/Scene.h"

namespace Ares
{
	class SceneRenderer
	{
	public:
		static void Init();
		static void CreateEnvironmentMap(const std::string& filepath);

		static void SetViewportSize(uint32_t width, uint32_t height);
		static void BeginScene(const Scene* scene);
		static void SubmitEntity(Entity* entity);
		static void EndScene();

		static void FlushDrawList();
		static Ref<Texture2D> GetFinalColorBuffer();
		static uint32_t GetFinalColorBufferRendererID();

		static void GeometryPass();
		static void CompositePass();
	private:

	};

}