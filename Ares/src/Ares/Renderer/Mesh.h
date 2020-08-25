#pragma once

#include <vector>
#include <string>
//#include <glm/glm.hpp>
#include "Ares/Math/Math.h"
#include "Ares/Renderer/VertexArray.h"
#include "Ares/Renderer/Material.h"
#include "Ares/Math/AABB.h"
#include "Ares/Renderer/Animation.h"
#include "Ares/Renderer/Mesh/ModelLoadingUtils.h"

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
		glm::vec2 Texcoord;
		glm::vec3 Normal{ 0 };
		glm::vec3 Tangent{ 0 };
	};
	struct AnimatedVertex
	{
		glm::vec3 Position;
		glm::vec2 Texcoord;
		glm::vec3 Normal{ 0 };
		glm::vec3 Tangent{ 0 };
		
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

	/*
	struct BoneInfo
	{
		glm::mat4 BoneOffset;
		glm::mat4 FinalTransformation;
	};
	*/

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
		AABB BoundingBox;
		std::string NodeName, MeshName;
	};

	class Mesh
	{
		friend class MeshRendererComponent;
	public:
		Mesh(const std::string& filename, std::vector<Ref<Material>>& m_Materials, std::vector<Ref<Animation>>& m_Animations);
		Mesh(PrimitiveMeshType primitiveType);
		~Mesh();
		//inline const bool IsAnimated() const { return m_IsAnimated; }
		//void OnUpdate();
		//void DumpVertexBuffer();
		std::vector<Submesh>& GetSubmeshes() { return m_Submeshes; }
		const std::vector<Submesh>& GetSubmeshes() const { return m_Submeshes; }
		inline const std::string& GetFilePath() const { return m_FilePath; }

		const std::vector<Triangle> GetTriangleCache(uint32_t index) const { return m_TriangleCache.at(index); }

		inline const uint32_t GetBoneCount() const { return m_BoneCount; }
		inline Ref<ModelNode> GetRootNode() { return m_RootNode; }

	private:
		//void BoneTransform(float time);
		//void ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);
		void TraverseNodes(aiNode* node, const glm::mat4& parentTransform = glm::mat4(1.0f), uint32_t level = 0);

		Ref<ModelNode> m_RootNode;


		//const aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const std::string& nodeName);
		/*
		uint32_t FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
		uint32_t FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
		uint32_t FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
		glm::vec3 InterpolateTranslation(float animationTime, const aiNodeAnim* nodeAnim);
		glm::quat InterpolateRotation(float animationTime, const aiNodeAnim* nodeAnim);
		glm::vec3 InterpolateScale(float animationTime, const aiNodeAnim* nodeAnim);
		*/

		//uint32_t FindPosition(float AnimationTime, const AnimationNode& pNodeAnim);
		//uint32_t FindRotation(float AnimationTime, const AnimationNode& pNodeAnim);
		//uint32_t FindScaling(float AnimationTime, const AnimationNode& pNodeAnim);

		/*
		glm::vec3 InterpolateTranslation(float animationTime, const AnimationNode& nodeAnim);
		glm::quat InterpolateRotation(float animationTime, const AnimationNode& nodeAnim);
		glm::vec3 InterpolateScale(float animationTime, const AnimationNode& nodeAnim);

		static void BuildAnimation(const aiNode* pNode, const aiAnimation* loadedAnim, Ref<Animation> animation);
		*/

		//const float GetAnimationDuration() const;


		std::vector<Submesh> m_Submeshes;
		//std::unique_ptr<Assimp::Importer> m_Importer;

		//glm::mat4 m_InverseTransform;

		
		uint32_t m_BoneCount = 0;
		//std::vector<BoneInfo> m_BoneInfo;
		//std::vector<Matrix4> m_BoneInfo;
		//std::unordered_map<std::string, uint32_t> m_BoneMapping;
		
		//Ref<Texture2D> m_BoneMatrixTexture = nullptr;
		//float* m_BoneMatrixData = nullptr;

		
		//const aiScene* m_Scene = nullptr;




		// Animation
		//std::vector<Ref<Animation>> m_Animations;

		


		//bool m_IsAnimated = false;
		//float m_AnimationTime = 0.0f;
		//float m_WorldTime = 0.0f;
		//float m_TimeMultiplier = 1.0f;
		//bool m_AnimationPlaying = true;

		std::vector<Vertex> m_StaticVertices;
		std::vector<AnimatedVertex> m_AnimatedVertices;
		std::vector<uint32_t> m_Indices;

		Ref<VertexArray> m_VertexArray;

		std::string m_FilePath;

		std::unordered_map<uint32_t, std::vector<Triangle>> m_TriangleCache;

		friend class Renderer;
		friend class SceneHierarchyPanel;
	};
}
