#include "AresPCH.h" 
#include "Mesh.h"
#include "Ares/Renderer/VertexArray.h"

namespace Ares {
	
	static void GetSphereVertInfo(std::vector<Vertex>& m_Vertices, std::vector<uint32_t>& m_Indices)
	{
		const float PI = 3.14159265359f;
		const float radius = .5f;
		const uint32_t SECTORS = 64;
		const uint32_t STACKS = 64;

		float lengthInv = 1.0f / radius;    // vertex normal
		float sectorStep = 2.0f * PI / (float)SECTORS;
		float stackStep = PI / (float)STACKS;
		
		m_Vertices.reserve((STACKS + 1) * (SECTORS + 1));
		for (int i = 0; i <= STACKS; i++)
		{
			float yAngle = PI * 0.5f - i * stackStep;  // starting from pi/2 to -pi/2
			float y = radius * sinf(yAngle);              // r * sin(u)
			float xz = radius * cosf(yAngle);             // r * cos(u)

			// add (sectorCount+1) vertices per stack
			// the first and last vertices have same position and normal, but different tex coords
			for (int j = 0; j <= SECTORS; j++)
			{
				float xAngle = j * sectorStep;           // starting from 0 to 2pi
				Vertex vertex;
				vertex.Position = glm::vec3(xz * cosf(xAngle), y, xz * sinf(xAngle));
				vertex.Normal = vertex.Position * lengthInv;
				vertex.Texcoord = 1.0f - glm::vec2((float)j / SECTORS, (float)i / STACKS);
				m_Vertices.push_back(vertex);
			}
		}
		
		for (int i = 0; i < STACKS; ++i)
		{
			int k1 = i * (SECTORS + 1);     // beginning of current stack
			int k2 = k1 + SECTORS + 1;      // beginning of next stack

			for (int j = 0; j < SECTORS; j++, k1++, k2++)
			{
				// 2 triangles per sector excluding first and last stacks
				// k1 => k2 => k1+1
				if (i != 0)
				{
					m_Indices.push_back(k1);
					m_Indices.push_back(k2);
					m_Indices.push_back(k1 + 1);
				}
				// k1+1 => k2 => k2+1
				if (i != (STACKS - 1))
				{
					m_Indices.push_back(k1 + 1);
					m_Indices.push_back(k2);
					m_Indices.push_back(k2 + 1);
				}
			}
		}
	}

	static void GetCubeVertInfo(std::vector<Vertex>& m_Vertices, std::vector<uint32_t>& m_Indices)
	{
		glm::vec3 points[] = {
			glm::vec3(-.5f, -.5f,  .5f),
			glm::vec3(.5f,  -.5f,  .5f),
			glm::vec3(.5f,  -.5f, -.5f),
			glm::vec3(-.5f, -.5f, -.5f),
			glm::vec3(-.5f,  .5f,  .5f),
			glm::vec3(.5f,   .5f,  .5f),
			glm::vec3(.5f,   .5f, -.5f),
			glm::vec3(-.5f,  .5f, -.5f)
		};

		glm::vec3 verts[] = {
			points[0], points[1], points[2], points[3], // Bottom
			points[7], points[4], points[0], points[3], // Left
			points[4], points[5], points[1], points[0], // Front
			points[6], points[7], points[3], points[2], // Back
			points[5], points[6], points[2], points[1], // Right
			points[7], points[6], points[5], points[4]  // Top
		};

		glm::vec3 normals[] = {
			{  0, -1,  0 }, // Bottom
			{ -1,  0,  0 }, // Left
			{  0,  0,  1 },	// Front
			{  0,  0, -1 }, // Back
			{  1,  0,  0 }, // Right
			{  0,  1,  0 }  // Top
		};

		glm::vec2 uv00 = { 0, 0 };
		glm::vec2 uv10 = { 1, 0 };
		glm::vec2 uv01 = { 0, 1 };
		glm::vec2 uv11 = { 1, 1 };
		glm::vec2 uvs[] = { uv01, uv11, uv10, uv00 };

		const uint32_t numVertices = 24;
		m_Vertices.reserve(numVertices);
		for (size_t i = 0; i < numVertices; i++)
		{
			Vertex vertex;
			vertex.Position = verts[i];
			vertex.Normal = normals[i / 4];
			vertex.Texcoord = uvs[i % 4];
			m_Vertices.push_back(vertex);
		}

		for (uint32_t i = 0; i < 6; i++)
		{
			uint32_t offset = 4 * i;
			m_Indices.push_back(offset + 3);
			m_Indices.push_back(offset + 1);
			m_Indices.push_back(offset + 0);
			m_Indices.push_back(offset + 3);
			m_Indices.push_back(offset + 2);
			m_Indices.push_back(offset + 1);
		}
	}

	static void GetQuadVertInfo(std::vector<Vertex>& m_Vertices, std::vector<uint32_t>& m_Indices)
	{
		glm::vec3 verts[] = {
			{ -.5f, -.5f, 0 },
			{  .5f, -.5f, 0 },
			{  .5f,  .5f, 0 },
			{ -.5f,  .5f, 0 }
		};
		m_Vertices.reserve(4);
		for (size_t i = 0; i < 4; i++)
		{
			Vertex vertex;
			vertex.Position = verts[i];
			vertex.Normal = glm::vec3(0, 0, 1);
			vertex.Texcoord = glm::vec2(verts[i]) + 1.0f;
			m_Vertices.push_back(vertex);
		}

		m_Indices = { 0, 1, 2, 2, 3, 0 };
	}

	Mesh::Mesh(PrimitiveMeshType primitiveType)
	{
		switch (primitiveType)
		{
		case PrimitiveMeshType::Cube:
			GetCubeVertInfo(m_Vertices, m_Indicies);
			break;
		case PrimitiveMeshType::Quad:
			GetQuadVertInfo(m_Vertices, m_Indicies);
			break;
		case PrimitiveMeshType::Sphere:
			GetSphereVertInfo(m_Vertices, m_Indicies);
			break;
		default:
			ARES_CORE_ASSERT(false, "Unknown Primitive Type!");
			break;
		}

		CalculateTangents();
		// CalculateNormals();

		Submesh& submesh = m_Submeshes.emplace_back();
		submesh.BaseVertex = 0;
		submesh.BaseIndex = 0;
		submesh.MaterialIndex = 0;
		submesh.IndexCount = (uint32_t)m_Indicies.size();
		submesh.NumVertices = m_Vertices.size();
		submesh.ModelNode = CreateRef<ModelNode>();

		CalculateAABB();
		
		SetSubmeshes({ submesh });
		
		FinalizeMesh();
		
	}

	void Mesh::CalculateTrianglesCache()
	{
		for (size_t m = 0; m < m_Submeshes.size(); m++)
		{
			for (size_t i = 0; i < m_Submeshes[m].IndexCount; i += 3)
			{
				m_TriangleCache[m].emplace_back(
					m_Vertices[m_Indicies[m_Submeshes[m].BaseIndex + i + 0] + (size_t)m_Submeshes[m].BaseVertex],
					m_Vertices[m_Indicies[m_Submeshes[m].BaseIndex + i + 1] + (size_t)m_Submeshes[m].BaseVertex],
					m_Vertices[m_Indicies[m_Submeshes[m].BaseIndex + i + 2] + (size_t)m_Submeshes[m].BaseVertex]
				);
			}
		}
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::FinalizeMesh()
	{
		// TODO: assert that sizes are ok
		
		m_VertexArray = VertexArray::Create();

		Ref<VertexBuffer> vb = VertexBuffer::Create(m_Vertices.data(), (uint32_t)(m_Vertices.size() * sizeof(Vertex)));
		vb->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float3, "a_Tangent" },
		});

		m_VertexArray->AddVertexBuffer(vb);

		if (m_VertexBoneData.size())
		{
			Ref<VertexBuffer> vbBones = VertexBuffer::Create(m_VertexBoneData.data(), (uint32_t)(m_VertexBoneData.size() * sizeof(VertexBoneData)));
			vbBones->SetLayout({
				{ ShaderDataType::Float4, "a_BoneIDs" },
				{ ShaderDataType::Float4, "a_BoneWeights" },
			});
			m_VertexArray->AddVertexBuffer(vbBones);
		}

		m_VertexArray->SetIndexBuffer(IndexBuffer::Create(m_Indicies.data(), (uint32_t)m_Indicies.capacity()));

		if (!m_VertexBoneData.size())
		{
			CalculateTrianglesCache();
		}
	}

	void Mesh::SetVertices(const std::vector<Vertex>& vertices)
	{
		m_Vertices = vertices;
	}
	void Mesh::SetVertexBoneData(const std::vector<VertexBoneData>& vertexBoneData)
	{
		m_VertexBoneData = vertexBoneData;
	}
	void Mesh::SetIndicies(const std::vector<uint32_t>& indicies)
	{
		m_Indicies = indicies;
	}
	void Mesh::SetSubmeshes(const std::vector<Submesh>& submeshes)
	{
		m_Submeshes = submeshes;
	}



	void Mesh::CalculateTangents()
	{
		for (size_t i = 0; i < m_Indicies.size(); i += 3) {

			Vertex& v0 = m_Vertices[m_Indicies[i + 0]];
			Vertex& v1 = m_Vertices[m_Indicies[i + 1]];
			Vertex& v2 = m_Vertices[m_Indicies[i + 2]];

			glm::vec3 edge1 = v1.Position - v0.Position;
			glm::vec3 edge2 = v2.Position - v0.Position;

			float deltaU1 = v1.Texcoord.x - v0.Texcoord.x;
			float deltaU2 = v2.Texcoord.x - v0.Texcoord.x;

			float deltaV1 = v1.Texcoord.y - v0.Texcoord.y;
			float deltaV2 = v2.Texcoord.y - v0.Texcoord.y;

			float dividend = (deltaU1 * deltaV2 - deltaU2 * deltaV1);
			float f = dividend == 0.0f ? 0.0f : 1.0f / dividend;

			glm::vec3 tangent = (deltaV2 * edge1 - deltaV1 * edge2) * f;
			//glm::vec3 bitangent = (-deltaU2 * edge1 - deltaU1 * edge2) * f;

			v0.Tangent += tangent;
			v1.Tangent += tangent;
			v2.Tangent += tangent;

			/*v0.Binormal += bitangent;
			v1.Binormal += bitangent;
			v2.Binormal += bitangent;*/
		}


		for (uint32_t i = 0; i < m_Vertices.size(); i++)
		{
			Vertex& v = m_Vertices[i];
			v.Tangent = glm::normalize(v.Tangent);
			//v.Binormal = glm::normalize(v.Binormal);
		}
	}

	void Mesh::CalculateNormals()
	{
		for (size_t i = 0; i < m_Indicies.size(); i += 3)
		{
			Vertex& v0 = m_Vertices[m_Indicies[i + 0]];
			Vertex& v1 = m_Vertices[m_Indicies[i + 1]];
			Vertex& v2 = m_Vertices[m_Indicies[i + 2]];

			glm::vec3 norm = glm::normalize(glm::cross(
				v1.Position - v0.Position,
				v2.Position - v0.Position
			));

			v0.Normal += norm;
			v1.Normal += norm;
			v2.Normal += norm;
		}
		for (size_t i = 0; i < m_Vertices.size(); i++)
		{
			Vertex& v = m_Vertices[i];
			v.Normal = glm::normalize(v.Normal);
		}
	}

	void Mesh::CalculateAABB()
	{
		if (m_VertexBoneData.size())
			return;

		for (int i = 0; i < m_Submeshes.size(); i++)
		{
			Submesh& submesh = m_Submeshes[i];
			auto& aabb = submesh.BoundingBox;
			aabb.Min = { FLT_MAX, FLT_MAX, FLT_MAX };
			aabb.Max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

			for (size_t i = 0; i < submesh.NumVertices; i++)
			{
				aabb.Min.x = glm::min(m_Vertices[submesh.BaseVertex + i].Position.x, aabb.Min.x);
				aabb.Min.y = glm::min(m_Vertices[submesh.BaseVertex + i].Position.y, aabb.Min.y);
				aabb.Min.z = glm::min(m_Vertices[submesh.BaseVertex + i].Position.z, aabb.Min.z);
				aabb.Max.x = glm::max(m_Vertices[submesh.BaseVertex + i].Position.x, aabb.Max.x);
				aabb.Max.y = glm::max(m_Vertices[submesh.BaseVertex + i].Position.y, aabb.Max.y);
				aabb.Max.z = glm::max(m_Vertices[submesh.BaseVertex + i].Position.z, aabb.Max.z);
			}
		}
	}


	void Mesh::DumpVertexBuffer()
	{
		// TODO: Convert to ImGui
		ARES_CORE_LOG("------------------------------------------------------");
		ARES_CORE_LOG("Vertex Buffer Dump");
		//ARES_CORE_LOG("Mesh: {0}", m_FilePath);
		for (size_t i = 0; i < m_Vertices.size(); i++)
		{
			auto& vertex = m_Vertices[i];
			ARES_CORE_LOG("Vertex: {0}", i);
			ARES_CORE_LOG("Position: {0}, {1}, {2}", vertex.Position.x, vertex.Position.y, vertex.Position.z);
			ARES_CORE_LOG("Normal: {0}, {1}, {2}", vertex.Normal.x, vertex.Normal.y, vertex.Normal.z);
			ARES_CORE_LOG("Tangent: {0}, {1}, {2}", vertex.Tangent.x, vertex.Tangent.y, vertex.Tangent.z);
			ARES_CORE_LOG("TexCoord: {0}, {1}", vertex.Texcoord.x, vertex.Texcoord.y);
			ARES_CORE_LOG("--");
		}
		ARES_CORE_LOG("------------------------------------------------------");
	}
}