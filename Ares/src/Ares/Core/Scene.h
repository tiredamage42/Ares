#pragma once

#include "entt.hpp"
#include "Ares/Core/UUID.h"
#include "Ares/Renderer/Camera.h"
#include "Ares/Renderer/Material.h"
#include "Ares/Renderer/Renderer.h"
#include "Ares/Renderer/SceneCamera.h"

namespace Ares
{
	class Entity;
	using EntityMap = std::unordered_map<UUID, Entity>;
	
	struct Light
	{
		glm::vec3 Direction{ 0 };
		glm::vec3 Radiance{ 0 };
		float Multiplier = 1.0f;
	};
	class Scene
	{
		friend class SceneRenderer;
		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
		friend class EditorLayer;

	public:
		Scene(const std::string& debugName = "Scene");
		~Scene();

		void Init();

		void OnUpdate();
		void OnRenderRuntime();
		void OnRenderEditor(const Camera& editorCamera, const glm::mat4& viewMatrix, const Entity& selectedEntity);
		void OnEvent(Event& e);

		// Runtime
		void OnRuntimeStart();
		void OnRuntimeStop();

		Entity FindEntityByTag(const std::string& tag);

		float GetExposure() const { return m_Exposure; }
		float& GetExposure() { return m_Exposure; }

		void SetSkyboxMaterial(Ref<Material> skybox) { 
			m_SkyboxMaterial = skybox; 
			UpdateGI();
		}
		void UpdateGI();
		
		Light& GetLight() { return m_Light; }
		const Light& GetLight() const { return m_Light; }

		Entity GetMainCameraEntity();

		Entity CreateEntity(const std::string& name = "");
		Entity CreateEntityWithID(UUID uuid, const std::string& name = "", bool runtimeMap = false);

		void DestroyEntity(Entity entity);
		void DuplicateEntity(Entity entity);

		template<typename T>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<T>();
		}

		void OnViewportResize(uint32_t width, uint32_t height);

		void ForEachEntity(std::function<void(const Entity&)> foreach);

		const EntityMap& GetEntityMap() const { return m_EntityIDMap; }
		void CopyTo(Ref<Scene>& target);

		UUID GetUUID() const { return m_SceneID; }

		static Ref<Scene> GetScene(UUID uuid);

	private:
		UUID m_SceneID;
		
		// cotnainer for the actual component data (IDs)
		entt::registry m_Registry;
		EntityMap m_EntityIDMap;

		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		std::string m_DebugName;

		Light m_Light{};

		Ref<Material> m_SkyboxMaterial = nullptr;

		float m_Exposure = 0.8f;
		bool m_IsPlaying = false;

		
	};

}
