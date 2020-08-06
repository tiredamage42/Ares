#pragma once

#include "Ares/Renderer/RenderCommand.h"
#include "Ares/Renderer/Camera.h"
#include "Ares/Renderer/Shader.h"
#include "Ares/Renderer/RenderCommandQueue.h"

#include "Ares/Renderer/RenderPass.h"
#include "Ares/Renderer/Mesh.h"

namespace Ares {

	class Renderer
	{
	public:
		typedef void(*RenderCommandFn)(void*);

		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

		template<typename FuncT>
		static void Submit(FuncT&& func)
		{
			auto renderCmd = [](void* ptr) {
				auto pFunc = (FuncT*)ptr;
				(*pFunc)();
				// NOTE: Instead of destroying we could try and enforce all items to be trivally destructible
				// however some items like uniforms which contain std::strings still exist for now
				// static_assert(std::is_trivially_destructible_v<FuncT>, "FuncT must be trivially destructible");
				pFunc->~FuncT();
			};
			auto storageBuffer = s_CommandQueue.Allocate(renderCmd, sizeof(func));
			new (storageBuffer) FuncT(std::forward<FuncT>(func));
		}

		static void Clear(float r, float g, float b, float a);
		static void WaitAndRender();
		static void DrawIndexed(uint32_t count, bool depthTest = true);


		// ~Actual~ Renderer here... TODO: remove confusion later
		static void BeginRenderPass(const Ref<RenderPass>& renderPass);
		static void EndRenderPass();
		static void SubmitMesh(const Ref<Mesh>& mesh);


		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static Scope<SceneData> s_SceneData;
		static RenderCommandQueue s_CommandQueue;
		static Ref<RenderPass> s_ActiveRenderPass;


	};
}
