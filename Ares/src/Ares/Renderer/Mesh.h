#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "Ares/Renderer/VertexArray.h"

namespace Ares {

	class Mesh
	{
	public:
		struct Vertex
		{
			glm::vec3 Position;
			glm::vec3 Normal;
			glm::vec3 Tangent;
			glm::vec3 Binormal;
			glm::vec2 Texcoord;
		};
		static_assert(sizeof(Vertex) == 14 * sizeof(float));
		static const int NumAttributes = 5;

		/*struct Index
		{
			uint32_t V1, V2, V3;
		};*/
		//static_assert(sizeof(Index) == 3 * sizeof(uint32_t));

		Mesh(const std::string& filename);
		Mesh();
		~Mesh();

		void Render();

		inline const std::string& GetFilePath() const { return m_FilePath; }
	private:
		std::vector<Vertex> m_Vertices;
		//std::vector<Index> m_Indices;
		std::vector<uint32_t> m_Indices;

		Ref<VertexArray> m_VertexArray;
		/*std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;*/

		std::string m_FilePath;
	};



	/*
	
	s_Data.QuadVertexArray = VertexArray::Create();

		// VERTEX BUFFER =============================================================================
		s_Data.QuadVertexBuffer = VertexBuffer::Create(Renderer2DData::MAX_VERTS * sizeof(QuadVertex));

		s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float3, "a_UV" }
		});

		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		s_Data.QuadVertexBufferBase = new QuadVertex[Renderer2DData::MAX_VERTS];
		
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
	
	
	*/
}
