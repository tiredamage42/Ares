#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "Ares/Renderer/VertexArray.h"
#include "Ares/Renderer/Material.h"
#include "Ares/Math/AABB.h"

struct aiNode;
struct aiAnimation;
struct aiNodeAnim;
struct aiScene;

namespace Assimp {
	class Importer;
}

namespace Ares {

	enum class PrimitiveMeshType
	{
		Quad, Cube, Sphere
	};

	struct Vertex
	{

		glm::vec3 Position;
		glm::vec3 Normal{ 0 };
		glm::vec3 Tangent{ 0 };
		glm::vec3 Binormal;
		glm::vec2 Texcoord;
	};
	struct AnimatedVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal{ 0 };
		glm::vec3 Tangent{ 0 };
		glm::vec3 Binormal;
		glm::vec2 Texcoord;

		//uint32_t IDs[4] = { 0, 0, 0, 0 };
		float IDs[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		float Weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

		//void AddBoneData(uint32_t BoneID, float Weight)
		void AddBoneData2(float BoneInterpolation, float Weight)
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

	struct BoneInfo
	{
		glm::mat4 BoneOffset;
		glm::mat4 FinalTransformation;
	};

	//struct VertexBoneData
	//{
	//	//uint32_t IDs[4];
	//	float IDs[4];
	//	float Weights[4];

	//	VertexBoneData()
	//	{
	//		memset(IDs, 0, sizeof(IDs));
	//		memset(Weights, 0, sizeof(Weights));
	//	};

	//	//void AddBoneData(uint32_t BoneID, float Weight)
	//	void AddBoneData2(float BoneInterpolation, float Weight)
	//	{
	//		for (size_t i = 0; i < 4; i++)
	//		{
	//			if (Weights[i] == 0.0)
	//			{
	//				IDs[i] = BoneInterpolation;
	//				Weights[i] = Weight;
	//				return;
	//			}
	//		}

	//		// should never get here - more bones than we have space for
	//		ARES_CORE_ASSERT(false, "Too many bones!");
	//	}
	//};

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
		glm::mat4 Transform{ 1.0f };
		//glm::vec3 Min, Max; // TODO: AABB
		AABB BoundingBox;

		std::string NodeName, MeshName;
	};

	class Mesh
	{
	public:
		Mesh(const std::string& filename, std::vector<Ref<Material>>& m_Materials);
		Mesh(PrimitiveMeshType primitiveType);
		~Mesh();

		inline const bool IsAnimated() const { return m_IsAnimated; }

		//void Render(Ref<Shader> shader, const glm::mat4& transform = glm::mat4(1.0f));
		void OnUpdate();


		//void OnImGuiRender();
		void DumpVertexBuffer();

		std::vector<Submesh>& GetSubmeshes() { return m_Submeshes; }
		const std::vector<Submesh>& GetSubmeshes() const { return m_Submeshes; }


		//Ref<Material> GetMaterial() { return m_BaseMaterial; }
		
		//std::vector<Ref<Material>> GetMaterials() { return m_Materials; }
		//std::vector<Ref<MaterialInstance>> GetMaterialOverrides() { return m_MaterialOverrides; }

		
		//const std::vector<Ref<Texture2D>>& GetTextures() const { return m_Textures; }
		inline const std::string& GetFilePath() const { return m_FilePath; }

		const std::vector<Triangle> GetTriangleCache(uint32_t index) const { return m_TriangleCache.at(index); }
	private:
		void BoneTransform(float time);
		void ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);

		//void TraverseNodes(aiNode* node);
		void TraverseNodes(aiNode* node, const glm::mat4& parentTransform = glm::mat4(1.0f), uint32_t level = 0);


		const aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const std::string& nodeName);
		uint32_t FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
		uint32_t FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
		uint32_t FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
		glm::vec3 InterpolateTranslation(float animationTime, const aiNodeAnim* nodeAnim);
		glm::quat InterpolateRotation(float animationTime, const aiNodeAnim* nodeAnim);
		glm::vec3 InterpolateScale(float animationTime, const aiNodeAnim* nodeAnim);

		std::vector<Submesh> m_Submeshes;
		std::unique_ptr<Assimp::Importer> m_Importer;

		glm::mat4 m_InverseTransform;

		uint32_t m_BoneCount = 0;
		std::vector<BoneInfo> m_BoneInfo;
		
		Ref<Texture2D> m_BoneMatrixTexture = nullptr;
		float* m_BoneMatrixData = nullptr;

		std::unordered_map<std::string, uint32_t> m_BoneMapping;
		
		//std::vector<glm::mat4> m_BoneTransforms;

		const aiScene* m_Scene = nullptr;

		// Animation
		bool m_IsAnimated = false;
		float m_AnimationTime = 0.0f;
		float m_WorldTime = 0.0f;
		float m_TimeMultiplier = 1.0f;
		bool m_AnimationPlaying = true;

		std::vector<Vertex> m_StaticVertices;
		std::vector<AnimatedVertex> m_AnimatedVertices;
		std::vector<uint32_t> m_Indices;

		Ref<VertexArray> m_VertexArray;

		std::string m_FilePath;

		//Ref<Material> m_BaseMaterial;

		//std::vector<Ref<Material>> m_Materials;

		//std::vector<Ref<Texture2D>> m_Textures;
		//std::vector<Ref<Texture2D>> m_NormalMaps;
		//std::vector<Ref<MaterialInstance>> m_MaterialOverrides;

		std::unordered_map<uint32_t, std::vector<Triangle>> m_TriangleCache;



		friend class Renderer;
		friend class SceneHierarchyPanel;
	};
}
