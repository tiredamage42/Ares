#pragma once

#include <vector>
#include <string>
#include "Ares/Math/Math.h"
#include "Ares/Math/AABB.h"
#include "Ares/Renderer/Mesh/ModelNodeMap.h"

namespace Ares 
{
	class Material;
	class Animation;
	class VertexArray;

	enum class PrimitiveMeshType
	{
		Quad, Cube, Sphere
	};

	struct Vertex
	{
		Vector3 Position;
		Vector2 Texcoord;
		Vector3 Normal{ 0 };
		Vector3 Tangent{ 0 };
	};
	struct VertexBoneData
	{
		float IDs[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		float Weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

		void AddBoneData(float BoneInterpolation, float Weight)
		{
			for (size_t i = 0; i < 4; i++)
			{
				if (Weights[i] == 0.0)
				{
					IDs[i] = BoneInterpolation;
					Weights[i] = Weight;
					return;
				}
			}

			// TODO: Keep top weights
			ARES_CORE_WARN("Vertex has more than four bones/weights affecting it, extra data will be discarded (BoneID={0}, Weight={1})", BoneInterpolation, Weight);
		}
	};

	struct Triangle
	{
		Vertex V0, V1, V2;

		Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2)
			: V0(v0), V1(v1), V2(v2) {}
	};

	class Submesh
	{
	public:
		uint32_t BaseVertex;
		uint32_t BaseIndex;
		uint32_t MaterialIndex;
		uint32_t IndexCount;
		uint32_t NumVertices;
		Ref<ModelNode> ModelNode = nullptr;
		AABB BoundingBox;
	};

	class Mesh
	{
		friend class Renderer;
	public:
		Mesh(PrimitiveMeshType primitiveType);
		Mesh() = default;
		~Mesh();
		void DumpVertexBuffer();
		
		std::vector<Submesh>& GetSubmeshes() { return m_Submeshes; }
		const std::vector<Submesh>& GetSubmeshes() const { return m_Submeshes; }
		const std::vector<Triangle> GetTriangleCache(uint32_t index) const { return m_TriangleCache.at(index); }

		void SetVertices(const std::vector<Vertex>& vertices);
		void SetVertexBoneData(const std::vector<VertexBoneData>& vertexBoneData);
		void SetIndicies(const std::vector<uint32_t>& indicies);
		void SetSubmeshes(const std::vector<Submesh>& submeshes);
		void CalculateTangents();
		void CalculateNormals();
		void CalculateAABB();
		void FinalizeMesh();
		void CalculateTrianglesCache();
	private:
		std::unordered_map<uint32_t, std::vector<Triangle>> m_TriangleCache;
		std::vector<Submesh> m_Submeshes;
		std::vector<Vertex> m_Vertices;
		std::vector<VertexBoneData> m_VertexBoneData;
		std::vector<uint32_t> m_Indicies;
		Ref<VertexArray> m_VertexArray;
	};
}
