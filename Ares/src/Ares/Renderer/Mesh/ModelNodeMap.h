#pragma once
#include "Ares/Math/Math.h"
#include "Ares/Core/Core.h"
#include <string>
#include <vector>
namespace Ares
{
	struct ModelNode
	{
		std::string Name;
		Matrix4 DefaultLocalTransform{ 1.0f };
		bool IsBone = false;
		Matrix4 BoneOffset{ 1.0f };
		size_t BoneIndex;

		Ref<ModelNode> Parent = nullptr;
		std::vector<Ref<ModelNode>> Children;

		Matrix4 GetModelSpaceTransform() const
		{
			if (Parent)
			{
				return Parent->GetModelSpaceTransform() * DefaultLocalTransform;
			}
			else
			{
				return DefaultLocalTransform;
			}
		};
	};
	struct ModelNodeMap
	{
		uint32_t BoneCount;
		Ref<ModelNode> RootNode;
	};
}