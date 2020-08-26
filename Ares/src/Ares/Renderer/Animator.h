#pragma once
#include <tuple>
#include <vector>
#include "Animation.h"
#include "Ares/Core/Time.h"
#include "Ares/Renderer/Texture.h"
#include "Ares/Renderer/Mesh/ModelNodeMap.h"
namespace Ares
{
	/*
		BIG TODO:
			unlink the mesh dependency from this class...
	*/
	class Animator
	{
		friend class AnimatorComponent;
	public:
		Animator() = default;
		~Animator() = default;

		void UpdateBoneTransformsRecursive(float time, Ref<Animation> animation, Ref<ModelNode> modelNode, const glm::mat4& parentTransform);

		void UpdateBoneTransforms(float time, Ref<Animation> animation);
		void OnUpdate();
		void SetAnimationNodeInformation(Ref<ModelNodeMap> modelNodeMap);

		inline Ref<Texture2D> GetBoneTransformsTexture() { return m_BoneMatrixTexture; }
		inline void SetAnimations(std::vector<Ref<Animation>> animations) { m_Animations = animations; }
		
	private:
		Ref<Texture2D> m_BoneMatrixTexture = nullptr;
		float* m_BoneMatrixData = nullptr;

		Ref<ModelNodeMap> m_ModelNodeMap;
		Matrix4 m_InverseTransform;

		std::vector<Ref<Animation>> m_Animations;
		float m_Speed = 1.0f;
		int32_t m_CurrentAnimIndex = 0;
		bool m_AnimationPlaying = true;
		float m_AnimationTime = 0;
	};
}