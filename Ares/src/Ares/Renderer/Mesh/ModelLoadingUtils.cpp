#include "AresPCH.h"
#include "ModelLoadingUtils.h"
#include "Ares/Renderer/Animation.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/pbrmaterial.h>

namespace Ares
{
	

	static glm::mat4 Mat4FromAssimpMat4(const aiMatrix4x4& matrix)
	{
		glm::mat4 result;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		result[0][0] = matrix.a1; result[1][0] = matrix.a2; result[2][0] = matrix.a3; result[3][0] = matrix.a4;
		result[0][1] = matrix.b1; result[1][1] = matrix.b2; result[2][1] = matrix.b3; result[3][1] = matrix.b4;
		result[0][2] = matrix.c1; result[1][2] = matrix.c2; result[2][2] = matrix.c3; result[3][2] = matrix.c4;
		result[0][3] = matrix.d1; result[1][3] = matrix.d2; result[2][3] = matrix.d3; result[3][3] = matrix.d4;
		return result;
	}

	Ref<ModelNode> ModelLoadingUtils::CreateModelNodes(aiNode* aiNode, const std::vector<Matrix4>& boneInfo, const std::unordered_map<std::string, uint32_t>& boneMapping)
	{
		Ref<ModelNode> node = CreateRef<ModelNode>();
		node->DefaultLocalTransform = Mat4FromAssimpMat4(aiNode->mTransformation);
		node->Name = aiNode->mName.data;

		if (boneMapping.find(node->Name) != boneMapping.end())
		{
			uint32_t boneIndex = boneMapping.at(node->Name);
			node->IsBone = true;
			node->BoneIndex = boneIndex;
			node->BoneOffset = boneInfo[boneIndex];
		}

		for (uint32_t i = 0; i < aiNode->mNumChildren; i++)
		{
			Ref<ModelNode> child = CreateModelNodes(aiNode->mChildren[i], boneInfo, boneMapping);
			node->Children.push_back(child);
		}

		return node;
	}


	static void AddAnimationNode(const aiNode* pNode, const aiAnimation* loadedAnim, Ref<Animation> animation)
	{
		std::string nodeName(pNode->mName.data);

		const aiNodeAnim* nodeAnim = nullptr;

		for (uint32_t i = 0; i < loadedAnim->mNumChannels; i++)
		{
			if (std::string(loadedAnim->mChannels[i]->mNodeName.data) == nodeName)
			{
				nodeAnim = loadedAnim->mChannels[i];
				break;
			}
		}

		if (nodeAnim)
		{
			AnimationNode ares_animNode;

			for (uint32_t i = 0; i < nodeAnim->mNumPositionKeys; i++)
			{
				ares_animNode.Positions.push_back({
					(float)nodeAnim->mPositionKeys[i].mTime,
					{
						nodeAnim->mPositionKeys[i].mValue.x,
						nodeAnim->mPositionKeys[i].mValue.y,
						nodeAnim->mPositionKeys[i].mValue.z
					}
					});
			}
			for (uint32_t i = 0; i < nodeAnim->mNumRotationKeys; i++)
			{
				ares_animNode.Rotations.push_back({
					(float)nodeAnim->mRotationKeys[i].mTime,
					{
						nodeAnim->mRotationKeys[i].mValue.w,
						nodeAnim->mRotationKeys[i].mValue.x,
						nodeAnim->mRotationKeys[i].mValue.y,
						nodeAnim->mRotationKeys[i].mValue.z
					}
					});
			}
			for (uint32_t i = 0; i < nodeAnim->mNumScalingKeys; i++)
			{
				ares_animNode.Scales.push_back({
					(float)nodeAnim->mScalingKeys[i].mTime,
					{
						nodeAnim->mScalingKeys[i].mValue.x,
						nodeAnim->mScalingKeys[i].mValue.y,
						nodeAnim->mScalingKeys[i].mValue.z
					}
					});
			}

			animation->AddNode(nodeName, ares_animNode);
		}

		for (uint32_t i = 0; i < pNode->mNumChildren; i++)
			AddAnimationNode(pNode->mChildren[i], loadedAnim, animation);
	}

	Ref<Animation> ModelLoadingUtils::AssimpAnimation2AresAnimation(const aiScene* aiScene, const aiAnimation* aiAnimation)
	{
		Ref<Animation> animation = CreateRef<Animation>();
		animation->m_TicksPerSecond = aiAnimation->mTicksPerSecond;
		animation->m_Duration = aiAnimation->mDuration;
		AddAnimationNode(aiScene->mRootNode, aiAnimation, animation);
		return animation;
	}

}