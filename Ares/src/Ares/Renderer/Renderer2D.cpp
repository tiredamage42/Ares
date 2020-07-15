#include "AresPCH.h"
#include "Ares/Renderer/Renderer2D.h"
#include "Ares/Renderer/VertexArray.h"
#include "Ares/Renderer/Shader.h"

#include "Ares/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>


namespace Ares 
{
	struct Renderer2DStorage
	{
		Ref<VertexArray> VertexArray;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;
	};

	static Renderer2DStorage* s_Data;

	void Renderer2D::Init()
	{
		ARES_PROFILE_FUNCTION();

		s_Data = new Renderer2DStorage();

		s_Data->VertexArray = VertexArray::Create();

		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f,     0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f,     1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f,     1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f,     0.0f, 1.0f
		};

		Ref<VertexBuffer> squareVB = VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		squareVB->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_UV" }
		});
		s_Data->VertexArray->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Ref<IndexBuffer> squareIB = IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
		s_Data->VertexArray->SetIndexBuffer(squareIB);


		s_Data->WhiteTexture = Texture2D::Create(1, 1);

		uint32_t whiteTextureData = 0xffffffff;
		s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));


		s_Data->TextureShader = Shader::Create("Assets/Shaders/Texture.glsl");

		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetInt("u_Texture", 0);
	}

	void Renderer2D::Shutdown()
	{
		ARES_PROFILE_FUNCTION();

		delete s_Data;
	}
	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		ARES_PROFILE_FUNCTION();

		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetMat4("u_ViewProjectionMatrix", camera.GetViewProjectionMatrix());
	}
		

	void Renderer2D::EndScene()
	{
		ARES_PROFILE_FUNCTION();

	}
	//void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	//{
	//	DrawQuad({ position.x, position.y, 0.0f }, size, color);
	//}
	//void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	//{
	//	ARES_PROFILE_FUNCTION();

	//	s_Data->TextureShader->SetFloat4("u_Color", color);
	//	s_Data->WhiteTexture->Bind();
	//	// bind white texture

	//	glm::mat4 transform = 
	//		glm::translate(glm::mat4(1.0f), position) * 
	//		// rotation *
	//		glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	//	s_Data->TextureShader->SetMat4("u_TransformMatrix", transform);
	//	s_Data->VertexArray->Bind();
	//	RenderCommand::DrawIndexed(s_Data->VertexArray);
	//}
	void Renderer2D::DrawQuad(
		const glm::vec2& position, float rotation, const glm::vec2& size,
		const Ref<Texture2D>& texture, float tiling, const glm::vec4& color
	)
	{
		DrawQuad({ position.x, position.y, 0.0f }, rotation, size, texture, tiling, color);
	}
	void Renderer2D::DrawQuad(
		const glm::vec3& position, float rotation, const glm::vec2& size, 
		const Ref<Texture2D>& texture, float tiling, const glm::vec4& color
	)
	{
		ARES_PROFILE_FUNCTION();

		s_Data->TextureShader->SetFloat4("u_Color", color);
		s_Data->TextureShader->SetFloat("u_Tile", tiling);

		if (texture)
			texture->Bind();
		else
			s_Data->WhiteTexture->Bind();

		
		glm::mat4 transform = glm::mat4(1.0f);

		transform = glm::translate(transform, position);

		if (rotation != 0.0f)
			transform = glm::rotate(transform, rotation, { 0.0f, 0.0f, 1.0f });
		
		transform = glm::scale(transform, { size.x, size.y, 1.0f });

		//glm::mat4 transform =
		//	glm::translate(glm::mat4(1.0f), position) *
		//	// rotation *
		//	glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data->TextureShader->SetMat4("u_TransformMatrix", transform);

		

		s_Data->VertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->VertexArray);
	}
}