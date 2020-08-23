#pragma once
#include "Ares/Core/Core.h"
struct aiNode;
struct aiAnimation;
struct aiNodeAnim;
struct aiScene;
namespace Ares
{
	class Animation;
	class ModelLoadingUtils
	{
	public:
		static Ref<Animation> AssimpAnimation2AresAnimation(const aiScene* aiScene, const aiAnimation* aiAnimation);
	};

}