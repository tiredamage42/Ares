#pragma once
#include "Ares/Core/Core.h"
#include "Ares/Math/Math.h"
struct aiNode;
struct aiAnimation;
struct aiNodeAnim;
struct aiScene;
//struct aiMatrix4x4;

namespace Ares
{
	struct ModelNode
	{
		std::string Name;
		Matrix4 DefaultLocalTransform;
		bool IsBone;
		Matrix4 BoneOffset;
		uint32_t BoneIndex;
		std::vector<Ref<ModelNode>> Children;
	};

	class Animation;
	class ModelLoadingUtils
	{
	public:

		/*
		static Matrix4 Mat4FromAssimpMat4(const aiMatrix4x4& matrix);
		{
			glm::mat4 result;
			//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
			result[0][0] = matrix.a1; result[1][0] = matrix.a2; result[2][0] = matrix.a3; result[3][0] = matrix.a4;
			result[0][1] = matrix.b1; result[1][1] = matrix.b2; result[2][1] = matrix.b3; result[3][1] = matrix.b4;
			result[0][2] = matrix.c1; result[1][2] = matrix.c2; result[2][2] = matrix.c3; result[3][2] = matrix.c4;
			result[0][3] = matrix.d1; result[1][3] = matrix.d2; result[2][3] = matrix.d3; result[3][3] = matrix.d4;
			return result;
		}
		*/


		static Ref<Animation> AssimpAnimation2AresAnimation(const aiScene* aiScene, const aiAnimation* aiAnimation);

		static Ref<ModelNode> CreateModelNodes(aiNode* aiNode, const std::vector<Matrix4>& boneInfo, const std::unordered_map<std::string, uint32_t>& boneMapping);
		/*
		{
			ModelNode node;
			node.DefaultLocalTransform = Mat4FromAssimpMat4(aiNode->mTransformation);
			node.Name = aiNode->mName.data;

			if (boneMapping.find(node.Name) != boneMapping.end())
			{
				uint32_t boneIndex = boneMapping.at(node.Name);
				node.IsBone = true;
				node.BoneIndex = boneIndex;
				node.BoneOffset = boneInfo[boneIndex];
			}

			for (uint32_t i = 0; i < aiNode->mNumChildren; i++)
			{
				ModelNode& child = CreateModelNodes(aiNode->mChildren[i], boneInfo, boneMapping);
				node.Children.push_back(child);
			}

			return node;
		}

		*/
	};

}