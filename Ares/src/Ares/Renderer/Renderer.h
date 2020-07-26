#pragma once

#include "Ares/Renderer/RenderCommand.h"
#include "Ares/Renderer/Camera.h"
#include "Ares/Renderer/Shader.h"
#include "Ares/Renderer/RenderCommandQueue.h"


namespace Ares {

	class Renderer
	{
	public:
		
		static void Init();

		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();
		
		//static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform=glm::mat4(1.0f));

		static void Clear(float r, float g, float b, float a);
		static void WaitAndRender();


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