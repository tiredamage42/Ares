#include "AresPCH.h"
#include "Ares/Renderer/Renderer2D.h"
#include "Ares/Renderer/VertexArray.h"
#include "Ares/Renderer/Shader.h"

#include "Ares/Renderer/RenderCommand.h"


#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Ares 
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float Tiling;
	};

	struct Renderer2DData
	{
		const uint32_t MaxQuads = 10000;
		const uint32_t MaxVertices = MaxQuads * 4;
		const uint32_t MaxIndicies = MaxQuads * 6;

		static const uint32_t MaxTextureSlots = 32; // TODO: per platform
		
		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;

		
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;

		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		//stack allocated array
		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture

		glm::vec4 QuadVertices[4];

		
		Renderer2D::Statistics Stats;

	};


	static Renderer2DData s_Data;

	void Renderer2D::ResetStats()
	{

		//s_Data.Stats.DrawCalls = 0;
		//s_Data.Stats.QuadCount = 0;
		//s_Data.Stats.TextureCount = 0;
		memset(&s_Data.Stats, 0, sizeof(Renderer2D::Statistics));
	}
	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}
	
	void Renderer2D::Init()
	{
		ARES_PROFILE_FUNCTION();

		
		s_Data.QuadVertexArray = VertexArray::Create();

		/*float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f,     0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f,     1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f,     1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f,     0.0f, 1.0f
		};*/

		//Ref<VertexBuffer> squareVB = VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));

		s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_UV" },
			{ ShaderDataType::Float, "a_TexIndex" },
			{ ShaderDataType::Float, "a_Tiling" },
		});

		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);


		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];
		
		uint32_t* quadIndicies = new uint32_t[s_Data.MaxIndicies];
		

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndicies; i+=6)
		{
			quadIndicies[i + 0] = offset + 0;
			quadIndicies[i + 1] = offset + 1;
			quadIndicies[i + 2] = offset + 2;

			quadIndicies[i + 3] = offset + 2;
			quadIndicies[i + 4] = offset + 3;
			quadIndicies[i + 5] = offset + 0;

			offset += 4;
		}
		
		//uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndicies, s_Data.MaxIndicies);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndicies;


		s_Data.WhiteTexture = Texture2D::Create(1, 1);

		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));



		int32_t samplers[s_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;

		s_Data.TextureShader = Shader::Create("Assets/Shaders/Texture.glsl");

		s_Data.TextureShader->Bind();

		s_Data.TextureShader->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);
		//s_Data.TextureShader->SetInt("u_Texture", 0);

		// set all texture slots to 0
		/*for (uint32_t i = 0; i < s_Data.TextureSlots.size(); i++)
			s_Data.TextureSlots[i] = 0;*/


		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		s_Data.QuadVertices[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertices[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertices[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertices[3] = { -0.5f,  0.5f, 0.0f, 1.0f };


	}

	void Renderer2D::Shutdown()
	{
		ARES_PROFILE_FUNCTION();

		//delete s_Data;

		delete[] s_Data.QuadVertexBufferBase;

	}
	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		ARES_PROFILE_FUNCTION();

		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_ViewProjectionMatrix", camera.GetViewProjectionMatrix());

		s_Data.QuadIndexCount = 0;

		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}
	void Renderer2D::FlushAndReset()
	{
		EndScene();
		s_Data.QuadIndexCount = 0;

		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}


	void Renderer2D::EndScene()
	{
		ARES_PROFILE_FUNCTION();


		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);


		Flush();
	}

	void Renderer2D::Flush()
	{

		// Nothing to draw
		if (s_Data.QuadIndexCount == 0)
			return; 

		// bind textures
		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			s_Data.TextureSlots[i]->Bind(i);
		
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);

		s_Data.Stats.DrawCalls++;

	}
	
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

		if (s_Data.QuadIndexCount >= s_Data.MaxIndicies)
			FlushAndReset();
		
		float textureIndex = 0.0f;

		if (texture)
		{
			for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
			{
				// check if we've already submitted this texture
				if (*s_Data.TextureSlots[i].get() == *texture.get())
				{
					textureIndex = (float)i;
					break;
				}
			}


			if (textureIndex == 0.0f)
			{
				if (s_Data.TextureSlotIndex >= s_Data.MaxTextureSlots)
					FlushAndReset();

				textureIndex = (float)s_Data.TextureSlotIndex;
				s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;

				s_Data.TextureSlotIndex++;
			}
		}



		glm::mat4 transform = glm::mat4(1.0f);

		transform = glm::translate(transform, position);

		if (rotation != 0.0f)
			transform = glm::rotate(transform, rotation, { 0.0f, 0.0f, 1.0f });

		transform = glm::scale(transform, { size.x, size.y, 1.0f });

		for (uint32_t i = 0; i < 4; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertices[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = { (float)(i == 1 || i == 2), (float)(i > 1) };
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->Tiling = tiling;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}






	void Renderer2D::DrawQuad(
		const glm::vec2& position, float rotation, const glm::vec2& size,
		const Ref<SubTexture2D>& subTexture, float tiling, const glm::vec4& color
	)
	{
		DrawQuad({ position.x, position.y, 0.0f }, rotation, size, subTexture, tiling, color);
	}

	void Renderer2D::DrawQuad(
		const glm::vec3& position, float rotation, const glm::vec2& size,
		const Ref<SubTexture2D>& subTexture, float tiling, const glm::vec4& color
	)
	{
		ARES_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount >= s_Data.MaxIndicies)
			FlushAndReset();

		float textureIndex = 0.0f;

		const glm::vec2* textureCoords = nullptr;
		if (subTexture)
		{

			textureCoords = subTexture->GetTexCoords();

			for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
			{
				// check if we've already submitted this texture
				if (*s_Data.TextureSlots[i].get() == *(subTexture->GetTexture()).get())
				{
					textureIndex = (float)i;
					break;
				}
			}


			if (textureIndex == 0.0f)
			{
				if (s_Data.TextureSlotIndex >= s_Data.MaxTextureSlots)
					FlushAndReset();

				textureIndex = (float)s_Data.TextureSlotIndex;
				s_Data.TextureSlots[s_Data.TextureSlotIndex] = subTexture->GetTexture();

				s_Data.TextureSlotIndex++;
			}
		}

		glm::mat4 transform = glm::mat4(1.0f);

		transform = glm::translate(transform, position);

		if (rotation != 0.0f)
			transform = glm::rotate(transform, rotation, { 0.0f, 0.0f, 1.0f });

		transform = glm::scale(transform, { size.x, size.y, 1.0f });

		for (uint32_t i = 0; i < 4; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertices[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->Tiling = tiling;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}
}