#pragma once
#include <tuple>
#include <vector>

namespace Ares
{
	class Animator
	{
	public:

		inline static Vector3 InterpolateTranslation(float time, const AnimationNode& node)
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
		}
	
	private:

		template<typename K>
		inline static const uint32_t FindKeyIDX(float animTime, const std::vector<K>& keys)
		{
			for (size_t i = 0; i < keys.size() - 1; i++)
				if (animTime < keys[i + 1].Time)
					return i;
			return 0;
		}

		template <typename T, typename K>
		inline static std::tuple<T, T, float> GetInterpolationBounds(float time, const std::vector<K>& keys)
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
	};
}