#include "AresPCH.h"
#include "Ares/Renderer/Renderer.h"
#include "Ares/Renderer/Renderer2D.h"
namespace Ares {

	Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();
	
	void Renderer::SetClearColor(float r, float g, float b, float a)
	{
	}

	void Renderer::DrawIndexed(uint32_t count, bool depthTest)
	{
		Submit([=]() {
			RendererAPI::DrawIndexed(count, depthTest);
		});
	}

	void Renderer::WaitAndRender()
	{
		s_Data.m_CommandQueue.Execute();
	}

	void Renderer::BeginRenderPass(const Ref<RenderPass>& renderPass)
	{
		ARES_CORE_ASSERT(renderPass, "Render pass cannot be null!");

		// todo: convert all of this into a render command buffer

		s_Data.m_ActiveRenderPass = renderPass;
		renderPass->GetSpecs().TargetFrameBuffer->Bind();
		const glm::vec4& clearColor = renderPass->GetSpecs().TargetFrameBuffer->GetSpecs().ClearColor;

		Renderer::Submit([=](){
			RendererAPI::Clear(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
		});
	}

	void Renderer::EndRenderPass()
	{
		ARES_CORE_ASSERT(s_Data.m_ActiveRenderPass, "No active render pass! have you called Renderer::EndRenderPass twice?");
		s_Data.m_ActiveRenderPass->GetSpecs().TargetFrameBuffer->Unbind();
		s_Data.m_ActiveRenderPass = nullptr;
	}

	void Renderer::SubmitQuad(const Ref<MaterialInstance>& material, const glm::mat4& transform)
	{
		bool depthTest = true;
		if (material)
		{

		}
	}

	void Renderer::Init()
	{
		ARES_PROFILE_FUNCTION();

		RenderCommand::Init();
		Renderer2D::Init();
	}
	
	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();
		
		shader->SetMat4("u_ViewProjectionMatrix", s_SceneData->ViewProjectionMatrix);
		shader->SetMat4("u_TransformMatrix", transform);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

}