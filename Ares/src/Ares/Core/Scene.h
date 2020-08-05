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

		void OnViewportResize(uint32_t width, uint32_t height);

	private:
		// cotnainer for the actual component data (IDs)
		entt::registry m_Registry;

		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		friend class Entity;
	};

}
