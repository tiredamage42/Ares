#pragma once

#include "Ares/Core/Scene.h"
#include "Ares/Core/Components.h"
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
		const bool HasComponent() const
		{
			return m_Scene->m_Registry.has<T>(m_EntityHandle);
		}

		template <typename T, typename... Args>
		T* AddComponent(Args&&... args) const
		{
			ARES_CORE_ASSERT(!HasComponent<T>(), "Entity Already Has Component!");
			return &m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		// maybe try_get
		template <typename T>
		T* GetComponent() const
		{
			return m_Scene->m_Registry.try_get<T>(m_EntityHandle);
		}

		template <typename T, typename... Args>
		T* GetOrAddComponent(Args&&... args) const
		{
			return &m_Scene->m_Registry.get_or_emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template <typename T>
		const void RemoveComponent() const
		{
			return m_Scene->m_Registry.remove_if_exists<T>(m_EntityHandle);
		}
		
		//glm::mat4& Transform() { return m_Scene->m_Registry.get<TransformComponent>(m_EntityHandle); }
		//const glm::mat4& Transform() const { return m_Scene->m_Registry.get<TransformComponent>(m_EntityHandle); }
		
		//const std::string& GetName() const { return m_Scene->m_Registry.get<EntityComponent>(m_EntityHandle).Name; }
		//const std::string& GetName() const { return m_Scene->m_Registry.get<TagComponent>(m_EntityHandle).Tag; }

		operator uint32_t () const { return (uint32_t)m_EntityHandle; }
		operator entt::entity() const { return m_EntityHandle; }
		operator bool() const { return m_EntityHandle != entt::null && m_Scene; }

		bool operator==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

		//UUID GetUUID() { return m_Scene->m_Registry.get<IDComponent>(m_EntityHandle).ID; }
		UUID GetSceneUUID() { return m_Scene->GetUUID(); }
	private:

		// not ref counted since entity shouldnt
		// have any "ownership" over scene
		Scene* m_Scene = nullptr;

		entt::entity m_EntityHandle{ entt::null };
		friend class SceneSerializer;
		friend class Scene;
	};

	
}
