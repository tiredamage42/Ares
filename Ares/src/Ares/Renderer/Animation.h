#pragma once
#include "Ares/Math/Math.h"
#include <unordered_map>
namespace Ares
{
	template<typename T>
	struct AnimationKey
	{
		float Time;
		T Value;
	};

	struct PositionKey : AnimationKey<Vector3>{};
	struct ScaleKey : AnimationKey<Vector3>{};
	struct RotationKey : AnimationKey<Quaternion>{};

	struct AnimationNode
	{
		std::vector<PositionKey> Positions;
		std::vector<RotationKey> Rotations;
		std::vector<ScaleKey> Scales;
	};

	class Animation
	{
		//friend class Mesh;
		friend class Animator;
		friend class ModelLoadingUtils;
		friend class AnimatorComponent;
	public:
		void AddNode(const std::string& key, const AnimationNode& node)
		{
			// TODO: assert key doesnt exist...
			m_Name2Node[key] = node;
		}
		const AnimationNode& FindNode(const std::string& key, bool& success)
		{
			if (m_Name2Node.find(key) != m_Name2Node.end())
			{
				success = true;
				return m_Name2Node.at(key);
			}
			success = false;
			return {};
		}
	private:
		std::string m_Name;
		float m_TicksPerSecond;
		float m_Duration;

		std::unordered_map<std::string, AnimationNode> m_Name2Node;
	};
}