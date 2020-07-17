#pragma once

#include "Ares/Renderer/RenderCommand.h"
#include "Ares/Renderer/Camera.h"
#include "Ares/Renderer/Shader.h"
namespace Ares {

	class Renderer
	{
	public:


		static void SetClearColor(float r, float g, float b, float a);
		static void DrawIndexed(uint32_t count, bool depthTest);
		static void WaitAndRender();
		static void BeginRenderPass(const Ref<RenderPass>& renderPass);
		static void EndRenderPass();


		static void SubmitQuad(const Ref<MaterialInstance>& material, const glm::mat4& transform);



		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform=glm::mat4(1.0f));

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static Scope <SceneData> s_SceneData;
	};
}