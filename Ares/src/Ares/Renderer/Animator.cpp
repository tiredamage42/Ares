#include "AresPCH.h"
#include "Animator.h"
#include "Ares/Math/Math.h"

namespace Ares
{

	template<typename K>
	static const uint32_t FindKeyIDX(float animTime, const std::vector<K>& keys)
	{
		for (size_t i = 0; i < keys.size() - 1; i++)
			if (animTime < keys[i + 1].Time)
				return i;
		return 0;
	}

	template <typename T, typename K>
	static std::tuple<T, T, float> GetInterpolationBounds(float time, const std::vector<K>& keys)
	{
		uint32_t size = keys.size();
		if (size == 1)
		{
			// No interpolation necessary for single value
			T v = keys[0].Value;
			return { v, v, 0 };
		}

		uint32_t idx0 = FindKeyIDX<K>(time, keys);
		uint32_t idx1 = idx0 + 1;
		float DeltaTime = keys[idx1].Time - keys[idx0].Time;
		float Factor = glm::clamp((time - keys[idx0].Time) / DeltaTime, 0.0f, 1.0f);
		return { keys[idx0].Value, keys[idx1].Value, Factor };
	}

	static Vector3 InterpolateTranslation(float time, const AnimationNode& node)
	{
		auto [Start, End, Factor] = GetInterpolationBounds<Vector3, PositionKey>(time, node.Positions);
		return Start + Factor * (End - Start);
	}

	static Quaternion InterpolateRotation(float time, const AnimationNode& node)
	{
		auto [Start, End, Factor] = GetInterpolationBounds<Quaternion, RotationKey>(time, node.Rotations);
		return glm::normalize(glm::slerp(Start, End, Factor));
	}

	static Vector3 InterpolateScale(float time, const AnimationNode& node)
	{
		auto [Start, End, Factor] = GetInterpolationBounds<Vector3, ScaleKey>(time, node.Scales);
		return Start + Factor * (End - Start);
	}


	void Animator::UpdateBoneTransformsRecursive(float time, Ref<Animation> animation, Ref<ModelNode> modelNode, const glm::mat4& parentTransform)
	{
		glm::mat4 localTransform = modelNode->DefaultLocalTransform;

		bool success;
		const AnimationNode& animNode = animation->FindNode(modelNode->Name, success);
		if (success)
		{
			glm::vec3 translation = InterpolateTranslation(time, animNode);
			glm::quat rotation = InterpolateRotation(time, animNode);
			glm::vec3 scale = InterpolateScale(time, animNode);
			localTransform = Math::GetTRSMatrix(translation, rotation, scale);
		}

		glm::mat4 globalTransform = parentTransform * localTransform;

		if (modelNode->IsBone)
		{
			Matrix4 finalTransform = m_InverseTransform * globalTransform * modelNode->BoneOffset;
			const float* pSource = (const float*)glm::value_ptr(finalTransform);
			std::copy(pSource, pSource + 16, m_BoneMatrixData + 16 * modelNode->BoneIndex);
		}

		for (auto& child : modelNode->Children)
			UpdateBoneTransformsRecursive(time, animation, child, globalTransform);
		
	}


	void Animator::UpdateBoneTransforms(float time, Ref<Animation> animation)
	{
		UpdateBoneTransformsRecursive(time, animation, m_ModelNodeMap->RootNode, glm::mat4(1.0f));
		m_BoneMatrixTexture->SetData(m_BoneMatrixData);
	}


	const float DEFAULT_TICKS_PER_SECOND = 25.0f;
	void Animator::OnUpdate()
	{
		Ref<Animation> currentAnimation = nullptr;
		if (m_CurrentAnimIndex != -1)
		{
			currentAnimation = m_Animations[m_CurrentAnimIndex];
		}
		if (m_AnimationPlaying && currentAnimation)
		{
			float ticksPerSecond = (currentAnimation->m_TicksPerSecond != 0 ? currentAnimation->m_TicksPerSecond : DEFAULT_TICKS_PER_SECOND) * m_Speed;
			m_AnimationTime += (float)Time::GetDeltaTime() * ticksPerSecond;
			m_AnimationTime = fmod(m_AnimationTime, currentAnimation->m_Duration);
		}

		UpdateBoneTransforms(m_AnimationTime, currentAnimation);
	}



	void Animator::SetAnimationNodeInformation(Ref<ModelNodeMap> modelNodeMap)
	{
		m_ModelNodeMap = modelNodeMap;

		m_BoneMatrixTexture = Texture2D::Create(TextureFormat::Float16, 4, modelNodeMap->BoneCount, TextureWrap::Clamp, FilterType::Point, false);

		if (m_BoneMatrixData)
		{
			delete[] m_BoneMatrixData;
		}
		m_BoneMatrixData = new float[16 * (size_t)modelNodeMap->BoneCount];

		m_InverseTransform = glm::inverse(modelNodeMap->RootNode->DefaultLocalTransform);
	}

}