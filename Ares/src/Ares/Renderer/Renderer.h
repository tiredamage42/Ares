#pragma once

#include "Ares/Renderer/RenderCommand.h"
#include "Ares/Renderer/Camera.h"
#include "Ares/Renderer/Shader.h"
#include "Ares/Renderer/RenderCommandQueue.h"


namespace Ares {

	class Renderer
	{
	public:
		typedef void(*RenderCommandFn)(void*);

		
		static void Init();

		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

		/*static void BeginScene(OrthographicCamera& camera);
		static void EndScene();*/
		
		//static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform=glm::mat4(1.0f));
		
		/*static void* Submit(RenderCommandFn fn, unsigned int size)
		{
			return s_CommandQueue.Allocate(fn, size);
		}*/

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
		static void DrawIndexed(uint32_t count);



		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static Scope <SceneData> s_SceneData;
		static RenderCommandQueue s_CommandQueue;

	};
}
//#define ARES_RENDER_PASTE2(a, b) a ## b
//#define ARES_RENDER_PASTE(a, b) ARES_RENDER_PASTE2(a, b)
//#define ARES_RENDER_UNIQUE(x) ARES_RENDER_PASTE(x, __LINE__)
//
//#define ARES_RENDER_1(arg0, code) \
//	do {\
//    struct ARES_RENDER_UNIQUE(ARESRenderCommand) \
//    {\
//		ARES_RENDER_UNIQUE(ARESRenderCommand)(typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0) \
//		: arg0(arg0) {}\
//		\
//        static void Execute(void* self)\
//        {\
//			auto& arg0 = ((ARES_RENDER_UNIQUE(ARESRenderCommand)*)self)->arg0;\
//            code\
//        }\
//		\
//		typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0;\
//    };\
//	{\
//		auto mem = ::Ares::Renderer::Submit(ARES_RENDER_UNIQUE(ARESRenderCommand)::Execute, sizeof(ARES_RENDER_UNIQUE(ARESRenderCommand)));\
//		new (mem) ARES_RENDER_UNIQUE(ARESRenderCommand)(arg0);\
//	} } while(0)
//
//
//#define ARES_RENDER_4(arg0, arg1, arg2, arg3, code) \
//    struct ARES_RENDER_UNIQUE(ARESRenderCommand) \
//    {\
//		ARES_RENDER_UNIQUE(ARESRenderCommand)(typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0,\
//											typename ::std::remove_const<typename ::std::remove_reference<decltype(arg1)>::type>::type arg1,\
//											typename ::std::remove_const<typename ::std::remove_reference<decltype(arg2)>::type>::type arg2,\
//											typename ::std::remove_const<typename ::std::remove_reference<decltype(arg3)>::type>::type arg3)\
//		: arg0(arg0), arg1(arg1), arg2(arg2), arg3(arg3) {}\
//		\
//        static void Execute(void* self)\
//        {\
//			auto& arg0 = ((ARES_RENDER_UNIQUE(ARESRenderCommand)*)self)->arg0;\
//			auto& arg1 = ((ARES_RENDER_UNIQUE(ARESRenderCommand)*)self)->arg1;\
//			auto& arg2 = ((ARES_RENDER_UNIQUE(ARESRenderCommand)*)self)->arg2;\
//			auto& arg3 = ((ARES_RENDER_UNIQUE(ARESRenderCommand)*)self)->arg3;\
//            code\
//        }\
//		\
//		typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0;\
//		typename ::std::remove_const<typename ::std::remove_reference<decltype(arg1)>::type>::type arg1;\
//		typename ::std::remove_const<typename ::std::remove_reference<decltype(arg2)>::type>::type arg2;\
//		typename ::std::remove_const<typename ::std::remove_reference<decltype(arg3)>::type>::type arg3;\
//    };\
//	{\
//		auto mem = Renderer::Submit(ARES_RENDER_UNIQUE(ARESRenderCommand)::Execute, sizeof(ARES_RENDER_UNIQUE(ARESRenderCommand)));\
//		new (mem) ARES_RENDER_UNIQUE(ARESRenderCommand)(arg0, arg1, arg2, arg3);\
//	}