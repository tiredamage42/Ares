#pragma once

#include "Ares/Core/Scene.h"

#include "entt.hpp"

namespace Ares
{
	/*
		just a wrapper for convenience functions for entt

		(since entt::entity is just an ID not an actual class)
	*/
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity entityHandle, Scene* scene)
			: m_EntityHandle(entityHandle), m_Scene(scene)
		{

		}
		Entity(const Entity& other) = default;

		template <typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.has<T>(m_EntityHandle);
		}

		template <typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			ARES_CORE_ASSERT(!HasComponent<T>(), "Entity Already Has Component!");

			return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		// maybe try_get
		template <typename T>
		T& GetComponent()
		{
			ARES_CORE_ASSERT(HasComponent<T>(), "Entity Doesn't Have Component!");

			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template <typename T, typename... Args>
		T& GetOrAddComponent(Args&&... args)
		{
			return m_Scene->m_Registry.get_or_emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template <typename T>
		void RemoveComponent()
		{
			return m_Scene->m_Registry.remove_if_exists<T>(m_EntityHandle);
		}

		operator bool() const { return m_EntityHandle != entt::null; }



		
	private:

		// not ref counted since entity shouldnt
		// have any "ownership" over scene
		Scene* m_Scene = nullptr;

		entt::entity m_EntityHandle{ entt::null };
	};

	
}
