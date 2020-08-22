#include "AresPCH.h"
#include "Ares/Renderer/Renderer2D.h"
#include "Ares/Renderer/VertexArray.h"
#include "Ares/Renderer/Shader.h"

#include "Ares/Renderer/Renderer.h"

//#include <glm/gtc/matrix_transform.hpp>
#include "Ares/Math/Math.h"

namespace Ares 
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec3 TexCoord;
	};
	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 CamRange;
	};


	struct Renderer2DData
	{
		static const uint32_t MAX_QUADS = 20000;
		static const uint32_t MAX_VERTS = MAX_QUADS * 4;
		static const uint32_t MAX_INDICIES = MAX_QUADS * 6;
		static const uint32_t MAX_TEXTURE_SLOTS = 32; // TODO: per platform
		
		static const uint32_t MAX_LINES = 10000;
		static const uint32_t MAX_LINE_VERTS = MAX_LINES * 2;
		static const uint32_t MAX_LINE_INDICIES = MAX_LINES * 6;

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> TextureShader;
		// Lines
		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<Shader> LineShader;

		glm::mat4 QuadVertices;

		struct DrawData
		{
			uint32_t QuadIndexCount = 0;
			QuadVertex* QuadVertexBufferBase = nullptr;
			QuadVertex* QuadVertexBufferPtr = nullptr;

			//stack allocated array
			std::array<Ref<Texture2D>, MAX_TEXTURE_SLOTS> TextureSlots;
			uint32_t TextureSlotIndex = 1; // 0 = white texture

			uint32_t LineIndexCount = 0;
			LineVertex* LineVertexBufferBase = nullptr;
			LineVertex* LineVertexBufferPtr = nullptr;
		};

		DrawData DrawDataDepthTest;
		DrawData DrawDataNonDepthTest;

		//uint32_t QuadIndexCount = 0;
		//QuadVertex* QuadVertexBufferBase = nullptr;
		//QuadVertex* QuadVertexBufferPtr = nullptr;

		////stack allocated array
		//std::array<Ref<Texture2D>, MAX_TEXTURE_SLOTS> TextureSlots;
		//uint32_t TextureSlotIndex = 1; // 0 = white texture



		//uint32_t LineIndexCount = 0;
		//LineVertex* LineVertexBufferBase = nullptr;
		//LineVertex* LineVertexBufferPtr = nullptr;

		glm::mat4 CameraViewProj;
		Vector3 CameraPosition;
		//bool DepthTest = true;

		
		Renderer2D::Statistics Stats;
	};


	static Renderer2DData s_Data;

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}
	
	void Renderer2D::Init()
	{
		s_Data.QuadVertexArray = VertexArray::Create();

		// VERTEX BUFFER =============================================================================
		s_Data.QuadVertexBuffer = VertexBuffer::Create(Renderer2DData::MAX_VERTS * sizeof(QuadVertex));

		s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float3, "a_UV" }
		});

		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);


		s_Data.DrawDataDepthTest.QuadVertexBufferBase = new QuadVertex[Renderer2DData::MAX_VERTS];
		s_Data.DrawDataNonDepthTest.QuadVertexBufferBase = new QuadVertex[Renderer2DData::MAX_VERTS];
		//s_Data.QuadVertexBufferBase = new QuadVertex[Renderer2DData::MAX_VERTS];

		
		// INDEX BUFFER =============================================================================
		uint32_t* quadIndicies = new uint32_t[Renderer2DData::MAX_INDICIES];
		uint32_t offset = 0;
		for (uint32_t i = 0; i < Renderer2DData::MAX_INDICIES; i+=6)
		{
			// quadIndicies = { 0, 1, 2, 2, 3, 0 };
			quadIndicies[i + 0] = offset + 0;
			quadIndicies[i + 1] = offset + 1;
			quadIndicies[i + 2] = offset + 2;

			quadIndicies[i + 3] = offset + 2;
			quadIndicies[i + 4] = offset + 3;
			quadIndicies[i + 5] = offset + 0;
			offset += 4;
		}
		
		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndicies, Renderer2DData::MAX_INDICIES);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndicies;

		// 2D SPRITE SHADER =============================================================================
		/*int32_t* samplers = new int32_t[Renderer2DData::MAX_TEXTURE_SLOTS];
		for (uint32_t i = 0; i < Renderer2DData::MAX_TEXTURE_SLOTS; i++)
			samplers[i] = i;*/

		s_Data.TextureShader = Shader::Find("Assets/Shaders/Renderer2D_Quad.glsl");
		//s_Data.TextureShader->Bind();
		//s_Data.TextureShader->SetIntArray("u_Textures", samplers, Renderer2DData::MAX_TEXTURE_SLOTS);
		
		// WHITE TEXTURE =============================================================================
		
		/*
		Ref<Texture2D> whiteTexture = Texture2D::Create(TextureFormat::RGBA, 1, 1, TextureWrap::Repeat, FilterType::Point, false);

		uint32_t whiteTextureData = 0xffffffff;

		//whiteTexture->SetData(&whiteTextureData);
		whiteTexture->Lock();
		whiteTexture->GetWriteableBuffer().Write(&whiteTextureData, sizeof(uint32_t));
		whiteTexture->Unlock();
		*/

		s_Data.DrawDataDepthTest.TextureSlots[0] = Renderer::GetWhiteTexture();// whiteTexture;
		s_Data.DrawDataNonDepthTest.TextureSlots[0] = Renderer::GetWhiteTexture();// whiteTexture;

		//s_Data.TextureSlots[0] = Renderer::GetWhiteTexture();// whiteTexture;


		s_Data.QuadVertices = { 
			{ -0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f,  0.5f, 0.0f, 1.0f },
			{ -0.5f,  0.5f, 0.0f, 1.0f } 
		};





		// Lines
		s_Data.LineShader = Shader::Find("Assets/Shaders/Renderer2D_Line.glsl");
		s_Data.LineVertexArray = VertexArray::Create();

		s_Data.LineVertexBuffer = VertexBuffer::Create(s_Data.MAX_LINE_VERTS * sizeof(LineVertex));
		s_Data.LineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_CamRange" }
		});
		s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);

		s_Data.DrawDataDepthTest.LineVertexBufferBase = new LineVertex[s_Data.MAX_LINE_VERTS];
		s_Data.DrawDataNonDepthTest.LineVertexBufferBase = new LineVertex[s_Data.MAX_LINE_VERTS];
		//s_Data.LineVertexBufferBase = new LineVertex[s_Data.MAX_LINE_VERTS];

		uint32_t* lineIndices = new uint32_t[s_Data.MAX_LINE_INDICIES];
		for (uint32_t i = 0; i < s_Data.MAX_LINE_INDICIES; i++)
			lineIndices[i] = i;

		Ref<IndexBuffer> lineIB = IndexBuffer::Create(lineIndices, s_Data.MAX_LINE_INDICIES);
		s_Data.LineVertexArray->SetIndexBuffer(lineIB);
		delete[] lineIndices;
	}

	void Renderer2D::Shutdown()
	{
		delete[] s_Data.DrawDataDepthTest.QuadVertexBufferBase;
		delete[] s_Data.DrawDataDepthTest.LineVertexBufferBase;
		delete[] s_Data.DrawDataNonDepthTest.QuadVertexBufferBase;
		delete[] s_Data.DrawDataNonDepthTest.LineVertexBufferBase;

		
		//delete[] s_Data.QuadVertexBufferBase;
		//delete[] s_Data.LineVertexBufferBase;
	}
	//void Renderer2D::BeginScene(const glm::mat4& projection, const glm::mat4& transform) 
	void Renderer2D::BeginScene(const glm::mat4& viewProj, const Vector3& cameraPosition)//, bool depthTest)
	{

		// reset stats
		memset(&s_Data.Stats, 0, sizeof(Renderer2D::Statistics));

		s_Data.CameraPosition = cameraPosition;
		s_Data.CameraViewProj = viewProj;
		//s_Data.DepthTest = depthTest;



		// from 2D
		//glm::mat4 viewProj = projection * glm::inverse(transform);

		// needded?
		//s_Data.TextureShader->Bind(ShaderVariant::Static);
		//s_Data.TextureShader->SetMat4("u_ViewProjection", viewProj, ShaderVariant::Static);


		ResetQuadsDepthTest();
		ResetQuadsNonDepthTest();
		ResetLinesDepthTest();
		ResetLinesNonDepthTest();
/*
		s_Data.DrawDataDepthTest.QuadIndexCount = 0;
		s_Data.DrawDataDepthTest.QuadVertexBufferPtr = s_Data.DrawDataDepthTest.QuadVertexBufferBase;
		s_Data.DrawDataDepthTest.TextureSlotIndex = 1;

		s_Data.DrawDataDepthTest.LineIndexCount = 0;
		s_Data.DrawDataDepthTest.LineVertexBufferPtr = s_Data.DrawDataDepthTest.LineVertexBufferBase;

		s_Data.DrawDataNonDepthTest.QuadIndexCount = 0;
		s_Data.DrawDataNonDepthTest.QuadVertexBufferPtr = s_Data.DrawDataNonDepthTest.QuadVertexBufferBase;
		s_Data.DrawDataNonDepthTest.TextureSlotIndex = 1;

		s_Data.DrawDataNonDepthTest.LineIndexCount = 0;
		s_Data.DrawDataNonDepthTest.LineVertexBufferPtr = s_Data.DrawDataNonDepthTest.LineVertexBufferBase;
*/

		
		/*
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.LineIndexCount = 0;
		s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
		*/
	}

	
	void Renderer2D::ResetQuadsDepthTest()
	{
		s_Data.DrawDataDepthTest.QuadIndexCount = 0;
		s_Data.DrawDataDepthTest.QuadVertexBufferPtr = s_Data.DrawDataDepthTest.QuadVertexBufferBase;
		s_Data.DrawDataDepthTest.TextureSlotIndex = 1;
	}
	void Renderer2D::ResetQuadsNonDepthTest()
	{
		s_Data.DrawDataNonDepthTest.QuadIndexCount = 0;
		s_Data.DrawDataNonDepthTest.QuadVertexBufferPtr = s_Data.DrawDataNonDepthTest.QuadVertexBufferBase;
		s_Data.DrawDataNonDepthTest.TextureSlotIndex = 1;
	}

	void Renderer2D::ResetLinesDepthTest()
	{
		s_Data.DrawDataDepthTest.LineIndexCount = 0;
		s_Data.DrawDataDepthTest.LineVertexBufferPtr = s_Data.DrawDataDepthTest.LineVertexBufferBase;
	}
	void Renderer2D::ResetLinesNonDepthTest()
	{
		s_Data.DrawDataNonDepthTest.LineIndexCount = 0;
		s_Data.DrawDataNonDepthTest.LineVertexBufferPtr = s_Data.DrawDataNonDepthTest.LineVertexBufferBase;
	}




	//void Renderer2D::BeginScene(const OrthographicCamera& camera)
	//{
	//	// reset stats
	//	memset(&s_Data.Stats, 0, sizeof(Renderer2D::Statistics));

	//	s_Data.TextureShader->Bind();
	//	s_Data.TextureShader->SetMat4("u_ViewProjectionMatrix", camera.GetViewProjectionMatrix());

	//	s_Data.QuadIndexCount = 0;
	//	s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
	//	s_Data.TextureSlotIndex = 1;
	//}

	/*
	void Renderer2D::FlushAndReset(bool quadDT, bool quadNDT, bool linesDT, bool linesNDT)
	{
		EndScene(quadDT, quadNDT, linesDT, linesNDT);
		if (quadDT)		ResetQuadsDepthTest();
		if (quadNDT)	ResetQuadsNonDepthTest();
		if (linesDT)	ResetLinesDepthTest();
		if (linesNDT)	ResetLinesNonDepthTest();

		/
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.TextureSlotIndex = 1;
		/
	}
	*/
	/*
	void Renderer2D::FlushAndResetLines()
	{
		/EndScene();
		s_Data.LineIndexCount = 0;
		s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;
		s_Data.TextureSlotIndex = 1;/
	}
	*/

	void Renderer2D::EndSceneQuadsDepthTest()
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.DrawDataDepthTest.QuadVertexBufferPtr - (uint8_t*)s_Data.DrawDataDepthTest.QuadVertexBufferBase);

		if (dataSize)
		{
			s_Data.QuadVertexBuffer->SetData(s_Data.DrawDataDepthTest.QuadVertexBufferBase, dataSize);

			// Nothing to draw
			/*if (s_Data.QuadIndexCount == 0)
				return; */

				//is this neeeded?
			s_Data.TextureShader->Bind(ShaderVariations::Default);
			s_Data.TextureShader->SetMat4("u_ViewProjection", s_Data.CameraViewProj);


			// bind textures
			for (uint32_t i = 0; i < s_Data.DrawDataDepthTest.TextureSlotIndex; i++)
				s_Data.DrawDataDepthTest.TextureSlots[i]->Bind(i);


			s_Data.QuadVertexArray->Bind();
			Renderer::DrawIndexed(s_Data.DrawDataDepthTest.QuadIndexCount, PrimitiveType::Triangles, true);

			s_Data.Stats.DrawCalls++;
		}

	}

	void Renderer2D::EndSceneQuadsNonDepthTest()
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.DrawDataNonDepthTest.QuadVertexBufferPtr - (uint8_t*)s_Data.DrawDataNonDepthTest.QuadVertexBufferBase);

		if (dataSize)
		{
			s_Data.QuadVertexBuffer->SetData(s_Data.DrawDataNonDepthTest.QuadVertexBufferBase, dataSize);

			// Nothing to draw
			/*if (s_Data.QuadIndexCount == 0)
				return; */

				//is this neeeded?
			s_Data.TextureShader->Bind(ShaderVariations::Default);
			s_Data.TextureShader->SetMat4("u_ViewProjection", s_Data.CameraViewProj);


			// bind textures
			for (uint32_t i = 0; i < s_Data.DrawDataNonDepthTest.TextureSlotIndex; i++)
				s_Data.DrawDataNonDepthTest.TextureSlots[i]->Bind(i);


			s_Data.QuadVertexArray->Bind();
			Renderer::DrawIndexed(s_Data.DrawDataNonDepthTest.QuadIndexCount, PrimitiveType::Triangles, false);

			s_Data.Stats.DrawCalls++;
		}
	}
	void Renderer2D::EndSceneLinesDepthTest()
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.DrawDataDepthTest.LineVertexBufferPtr - (uint8_t*)s_Data.DrawDataDepthTest.LineVertexBufferBase);
		if (dataSize)
		{
			s_Data.LineVertexBuffer->SetData(s_Data.DrawDataDepthTest.LineVertexBufferBase, dataSize);

			// is this neeeded?
			s_Data.LineShader->Bind(ShaderVariations::Default);
			s_Data.LineShader->SetMat4("u_ViewProjection", s_Data.CameraViewProj);
			s_Data.LineShader->SetFloat3("ares_CamPosition", s_Data.CameraPosition);



			s_Data.LineVertexArray->Bind();
			Renderer::SetLineThickness(2.0f);
			Renderer::DrawIndexed(s_Data.DrawDataDepthTest.LineIndexCount, PrimitiveType::Lines, true);
			s_Data.Stats.DrawCalls++;
		}
	}
	void Renderer2D::EndSceneLinesNonDepthTest()
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.DrawDataNonDepthTest.LineVertexBufferPtr - (uint8_t*)s_Data.DrawDataNonDepthTest.LineVertexBufferBase);
		if (dataSize)
		{
			s_Data.LineVertexBuffer->SetData(s_Data.DrawDataNonDepthTest.LineVertexBufferBase, dataSize);

			// is this neeeded?
			s_Data.LineShader->Bind(ShaderVariations::Default);
			s_Data.LineShader->SetMat4("u_ViewProjection", s_Data.CameraViewProj);
			s_Data.LineShader->SetFloat3("ares_CamPosition", s_Data.CameraPosition);



			s_Data.LineVertexArray->Bind();
			Renderer::SetLineThickness(2.0f);
			Renderer::DrawIndexed(s_Data.DrawDataNonDepthTest.LineIndexCount, PrimitiveType::Lines, false);
			s_Data.Stats.DrawCalls++;
		}
	}


	void Renderer2D::EndScene(bool quadDT, bool quadNDT, bool linesDT, bool linesNDT)
	{
		if (quadDT)		EndSceneQuadsDepthTest();
		if (quadNDT)	EndSceneQuadsNonDepthTest();
		if (linesDT)	EndSceneLinesDepthTest();
		if (linesNDT)	EndSceneLinesNonDepthTest();

		if (quadDT)		ResetQuadsDepthTest();
		if (quadNDT)	ResetQuadsNonDepthTest();
		if (linesDT)	ResetLinesDepthTest();
		if (linesNDT)	ResetLinesNonDepthTest();

		
		/*
		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
		
		if (dataSize)
		{
			s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

			// Nothing to draw
			/if (s_Data.QuadIndexCount == 0)
				return; /

			 //is this neeeded?
			s_Data.TextureShader->Bind(ShaderVariations::Default);
			s_Data.TextureShader->SetMat4("u_ViewProjection", s_Data.CameraViewProj);


			// bind textures
			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
				s_Data.TextureSlots[i]->Bind(i);
		

			s_Data.QuadVertexArray->Bind();
			Renderer::DrawIndexed(s_Data.QuadIndexCount, PrimitiveType::Triangles, s_Data.DepthTest);

			s_Data.Stats.DrawCalls++;	
		}

		dataSize = (uint32_t)((uint8_t*)s_Data.LineVertexBufferPtr - (uint8_t*)s_Data.LineVertexBufferBase);
		if (dataSize)
		{
			s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, dataSize);

			// is this neeeded?
			s_Data.LineShader->Bind(ShaderVariations::Default);
			s_Data.LineShader->SetMat4("u_ViewProjection", s_Data.CameraViewProj);
			s_Data.LineShader->SetFloat3("ares_CamPosition", s_Data.CameraPosition);
			


			s_Data.LineVertexArray->Bind();
			Renderer::SetLineThickness(2.0f);
			Renderer::DrawIndexed(s_Data.LineIndexCount, PrimitiveType::Lines, s_Data.DepthTest);
			s_Data.Stats.DrawCalls++;
		}
		*/
		
	}

	void Renderer2D::SubmitQuad(const glm::vec2& position, float rotation, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec2& tiling, const glm::vec2& offset, const glm::vec4& color, bool depthTest)
	{
		SubmitQuad({ position.x, position.y, 0.0f }, rotation, size, texture, tiling, offset, color, depthTest);
	}
	void Renderer2D::SubmitQuad(const glm::vec3& position, float rotation, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec2& tiling, const glm::vec2& offset, const glm::vec4& color, bool depthTest)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		
		if (rotation != 0.0f)
			transform = glm::rotate(transform, rotation, { 0.0f, 0.0f, 1.0f });
		
		SubmitQuad(glm::scale(transform, { size.x, size.y, 1.0f }), texture, tiling, offset, color, depthTest);
	}

	void Renderer2D::SubmitQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec2& tiling, const glm::vec2& offset, const glm::vec4& color, bool depthTest)
	{
		Renderer2DData::DrawData& drawData = depthTest ? s_Data.DrawDataDepthTest : s_Data.DrawDataNonDepthTest;

		if (drawData.QuadIndexCount / 6 > s_MaxQuadsPerDraw || drawData.QuadIndexCount >= Renderer2DData::MAX_INDICIES)
		{
			EndScene(depthTest, !depthTest, false, false);
			//FlushAndReset(depthTest, !depthTest, false, false);
		}

		float textureIndex = 0.0f;

		if (texture)
		{
			for (uint32_t i = 1; i < drawData.TextureSlotIndex; i++)
			{
				// check if we've already submitted this texture
				if (*drawData.TextureSlots[i].get() == *texture.get())
				{
					textureIndex = (float)i;
					break;
				}
			}

			if (textureIndex == 0.0f)
			{
				if (drawData.TextureSlotIndex >= Renderer2DData::MAX_TEXTURE_SLOTS)
				{
					EndScene(depthTest, !depthTest, false, false);
					//FlushAndReset(depthTest, !depthTest, false, false);
				}

				textureIndex = (float)drawData.TextureSlotIndex;
				drawData.TextureSlots[drawData.TextureSlotIndex] = texture;

				drawData.TextureSlotIndex++;
			}
		}

		glm::mat4 transformedVerts = transform * s_Data.QuadVertices;

		for (uint32_t i = 0; i < 4; i++)
		{
			drawData.QuadVertexBufferPtr->Position = transformedVerts[i];
			drawData.QuadVertexBufferPtr->Color = color;
			drawData.QuadVertexBufferPtr->TexCoord = {
				offset.x + tiling.x * (float)(i == 1 || i == 2),
				offset.y + tiling.y * (float)(i > 1),
				textureIndex
			};
			drawData.QuadVertexBufferPtr++;
		}

		drawData.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::SubmitLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, bool depthTest, const Vector2& cameraRange)
	{

		Renderer2DData::DrawData& drawData = depthTest ? s_Data.DrawDataDepthTest : s_Data.DrawDataNonDepthTest;

		if (drawData.LineIndexCount >= Renderer2DData::MAX_LINE_INDICIES)
		{
			EndScene(false, false, depthTest, !depthTest);
			//FlushAndReset(false, false, depthTest, !depthTest);
		}

		Vector2 cr = { cameraRange.x, cameraRange.y - cameraRange.x };

		drawData.LineVertexBufferPtr->Position = p0;
		drawData.LineVertexBufferPtr->Color = color;
		drawData.LineVertexBufferPtr->CamRange = cr;
		drawData.LineVertexBufferPtr++;
		
		drawData.LineVertexBufferPtr->Position = p1;
		drawData.LineVertexBufferPtr->Color = color;
		drawData.LineVertexBufferPtr->CamRange = cr;
		drawData.LineVertexBufferPtr++;

		drawData.LineIndexCount += 2;

		s_Data.Stats.LineCount++;
	}

	/*
	
	#type vertex
#version 430 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_CamRange;
uniform mat4 u_ViewProjection;

out vec4 v_Color;
out vec2 v_CamRange;
out vec3 v_WorldPos;

void main()
{
	v_Color = a_Color;
	v_CamRange = a_CamRange;
	v_WorldPos = a_Position;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 430 core

layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_CamRange;
in vec3 v_WorldPos;
uniform vec3 ares_CamPosition;

void main()
{
	color = v_Color;

	float dist = distance(ares_CamPosition - v_WorldPos);
	// fade out based on camera distance
	float alphaMultiplier = (dist - v_CamRange.x) / v_CamRange.y;

	color.a *= 1.0 - alphaMultiplier;
}
	*/
}