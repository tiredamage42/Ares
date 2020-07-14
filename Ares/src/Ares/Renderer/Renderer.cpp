#include "AresPCH.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"
namespace Ares {

	Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;
	
	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(
		const Ref<Shader>& shader, 
		const Ref<VertexArray>& vertexArray,
		const glm::mat4& transform
	)
	{
		shader->Bind();
		
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjectionMatrix", s_SceneData->ViewProjectionMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_TransformMatrix", transform);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

}