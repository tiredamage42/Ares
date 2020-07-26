#include "AresPCH.h"
#include "Ares/Renderer/Renderer.h"
#include "Ares/Renderer/Renderer2D.h"
namespace Ares {

	Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();
	
	RenderCommandQueue Renderer::s_CommandQueue;

	void Renderer::Init()
	{
		Submit([]() { RenderCommand::Init(); });
		/*RenderCommand::Init();*/
		Renderer2D::Init();
	}
	
	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
	}

	void Renderer::DrawIndexed(uint32_t count)
	{
		Renderer::Submit([=]() {
			RenderCommand::DrawIndexed(count);
		});
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{

		Renderer::Submit([=]() { 
		RenderCommand::SetViewport(0, 0, width, height);
			});

	}

	/*void Renderer::BeginScene(OrthographicCamera& camera)
	{
		s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}*/

	/*void Renderer::EndScene()
	{
	}*/

	/*void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();
		
		shader->SetMat4("u_ViewProjectionMatrix", s_SceneData->ViewProjectionMatrix);
		shader->SetMat4("u_TransformMatrix", transform);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}*/
	
	void Renderer::Clear(float r, float g, float b, float a)
	{
		/*float params[4] = { r, g, b, a };
		s_CommandQueue.SubmitCommand(RenderCommand::Clear, params, sizeof(float) * 4);*/

		Submit([=]() {
			RenderCommand::Clear(r, g, b, a);
		});
		
	}
	void Renderer::WaitAndRender()
	{
		s_CommandQueue.Execute();
	}
}