#pragma once
#include <tuple>
#include <vector>
#include "Animation.h"
//#include "Ares/Core/Components.h"
#include "Ares/Core/Time.h"
#include "Ares/Renderer/Mesh/ModelLoadingUtils.h"
#include "Ares/Renderer/Texture.h"
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

		
		/*inline static Vector3 InterpolateTranslation(float time, const AnimationNode& node)
		{
			auto [Start, End, Factor] = GetInterpolationBounds<Vector3, PositionKey>(time, node.Positions);
			return Start + Factor * (End - Start);
		}

		inline static Quaternion InterpolateRotation(float time, const AnimationNode& node)
		{
			auto [Start, End, Factor] = GetInterpolationBounds<Quaternion, RotationKey>(time, node.Rotations);
			return glm::normalize(glm::slerp(Start, End, Factor));
		}

		inline static Vector3 InterpolateScale(float time, const AnimationNode& node)
		{
			auto [Start, End, Factor] = GetInterpolationBounds<Vector3, ScaleKey>(time, node.Scales);
			return Start + Factor * (End - Start);
		}*/

		void UpdateBoneTransformsRecursive(float time, Ref<Animation> animation, Ref<ModelNode> modelNode, const glm::mat4& parentTransform);
		//{
		//	//std::string name(modelNode->mName.data);
		//	//glm::mat4 nodeTransform(Mat4FromAssimpMat4(modelNode->mTransformation));

		//	glm::mat4 localTransform = modelNode.DefaultLocalTransform;

		//	bool success;
		//	const AnimationNode& animNode = animation->FindNode(modelNode.Name, success);
		//	if (success)
		//	{
		//		glm::vec3 translation = Animator::InterpolateTranslation(time, animNode);
		//		glm::quat rotation = Animator::InterpolateRotation(time, animNode);
		//		glm::vec3 scale = Animator::InterpolateScale(time, animNode);
		//		localTransform = Math::GetTRSMatrix(translation, rotation, scale);
		//	}

		//	glm::mat4 globalTransform = parentTransform * localTransform;


		//	if (modelNode.IsBone)
		//	//if (m_BoneMapping.find(name) != m_BoneMapping.end())
		//	{
		//		//uint32_t BoneIndex = m_BoneMapping[name];
		//		// m_InverseTransform * globalTransform turns into object space
		//		//Matrix4 finalTransform = m_InverseTransform * globalTransform * m_BoneInfo[BoneIndex].BoneOffset;

		//		Matrix4 finalTransform = m_InverseTransform * globalTransform * modelNode.BoneOffset;

		//		const float* pSource = (const float*)glm::value_ptr(finalTransform);
		//		std::copy(pSource, pSource + 16, m_BoneMatrixData + 16 * modelNode.BoneIndex);
		//	}

		//	for (auto& child : modelNode.Children)
		//		UpdateBoneTransformsRecursive(time, animation, child, globalTransform);
		//	//for (uint32_t i = 0; i < modelNode->mNumChildren; i++)
		//		//UpdateBoneTransformsRecursive(time, animation, modelNode->mChildren[i], globalTransform);
		//}


		void UpdateBoneTransforms(float time, Ref<Animation> animation);
		//{
		//	UpdateBoneTransformsRecursive(time, animation, m_RootNode, glm::mat4(1.0f));
		//	m_BoneMatrixTexture->SetData(m_BoneMatrixData);
		//}


		//const float DEFAULT_TICKS_PER_SECOND = 25.0f;
		void OnUpdate();
		//{
		//	Ref<Animation> currentAnimation = nullptr;
		//	if (m_CurrentAnimIndex != -1)
		//	{
		//		currentAnimation = m_Animations[m_CurrentAnimIndex]; 
		//	}
		//	if (m_AnimationPlaying && currentAnimation)
		//	{
		//		//m_WorldTime += (float)Time::GetDeltaTime();

		//		float ticksPerSecond = (currentAnimation->m_TicksPerSecond != 0 ? currentAnimation->m_TicksPerSecond : DEFAULT_TICKS_PER_SECOND) * m_Speed;
		//		m_AnimationTime += (float)Time::GetDeltaTime() * ticksPerSecond;
		//		m_AnimationTime = fmod(m_AnimationTime, currentAnimation->m_Duration);
		//	}

		//	UpdateBoneTransforms(m_AnimationTime, currentAnimation);
		//}



		void SetAnimationNodeInformation(Ref<ModelNode> rootNode, uint32_t boneCount);
		//{
		//	m_RootNode = rootNode;

		//	m_BoneMatrixTexture = Texture2D::Create(TextureFormat::Float16, 4, boneCount, TextureWrap::Clamp, FilterType::Point, false);
		//	
		//	if (m_BoneMatrixData)
		//	{
		//		delete[] m_BoneMatrixData;
		//	}
		//	m_BoneMatrixData = new float[16 * (size_t)boneCount];

		//	m_InverseTransform = glm::inverse(rootNode.DefaultLocalTransform);
		//}

		inline Ref<Texture2D> GetBoneTransformsTexture()
		{
			return m_BoneMatrixTexture;
		}

		inline void SetAnimations(std::vector<Ref<Animation>> animations)
		{
			m_Animations = animations;
		}
		
	private:
		Ref<Texture2D> m_BoneMatrixTexture = nullptr;
		float* m_BoneMatrixData = nullptr;

		Ref<ModelNode> m_RootNode;
		Matrix4 m_InverseTransform;

		std::vector<Ref<Animation>> m_Animations;
		float m_Speed = 1.0f;
		int32_t m_CurrentAnimIndex = 0;
		bool m_AnimationPlaying = true;
		float m_AnimationTime;


		//template<typename K>
		//inline static const uint32_t FindKeyIDX(float animTime, const std::vector<K>& keys)
		//{
		//	for (size_t i = 0; i < keys.size() - 1; i++)
		//		if (animTime < keys[i + 1].Time)
		//			return i;
		//	return 0;
		//}

		//template <typename T, typename K>
		//inline static std::tuple<T, T, float> GetInterpolationBounds(float time, const std::vector<K>& keys)
		//{
		//	uint32_t size = keys.size();
		//	if (size == 1)
		//	{
		//		// No interpolation necessary for single value
		//		T v = keys[0].Value;
		//		return { v, v, 0 };
		//	}

		//	uint32_t idx0 = FindKeyIDX<K>(time, keys);
		//	uint32_t idx1 = idx0 + 1;
		//	float DeltaTime = keys[idx1].Time - keys[idx0].Time;
		//	float Factor = glm::clamp((time - keys[idx0].Time) / DeltaTime, 0.0f, 1.0f);
		//	return { keys[idx0].Value, keys[idx1].Value, Factor };
		//}
	};
}