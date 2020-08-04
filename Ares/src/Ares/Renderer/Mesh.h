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
	

		Mesh(const std::string& filename);
		Mesh();
		~Mesh();

		void Render();

		inline const std::string& GetFilePath() const { return m_FilePath; }
	private:
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;

		Ref<VertexArray> m_VertexArray;
		std::string m_FilePath;
	};
}
