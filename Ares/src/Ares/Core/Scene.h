#pragma once

#include "entt.hpp"
#include "Ares/Renderer/Camera.h"
namespace Ares
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		void OnUpdate();

		Entity CreateEntity(const std::string& name = std::string());

	private:
		// cotnainer for the actual component data (IDs)
		entt::registry m_Registry;

		friend class Entity;
	};

}
