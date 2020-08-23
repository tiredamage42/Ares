#include "AresPCH.h"
#include "ModelLoadingUtils.h"
#include "Ares/Renderer/Animation.h"

#include <assimp/scene.h>
//#include <assimp/postprocess.h>
//#include <assimp/Importer.hpp>
//#include <assimp/DefaultLogger.hpp>
//#include <assimp/LogStream.hpp>
//#include <assimp/pbrmaterial.h>


namespace Ares
{

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
		AddAnimationNode(aiScene->mRootNode, aiAnimation, animation);
		return animation;
	}

}