#pragma once

#include "entt.hpp"

#include "Ares/Core/UUID.h"

#include "Ares/Renderer/Camera.h"
#include "Ares/Renderer/Material.h"

#include "Ares/Renderer/SceneCamera.h"
#include "Ares/Editor/EditorCamera.h"

namespace Ares
{
	class Entity;
	using EntityMap = std::unordered_map<UUID, Entity>;

	struct Environment
	{
		std::string FilePath = "";
		Ref<TextureCube> RadianceMap = nullptr;
		Ref<TextureCube> IrradianceMap = nullptr;

		static Environment Load(const std::string& filepath);
	};
	struct Light
	{
		glm::vec3 Direction{ 0 };
		glm::vec3 Radiance{ 0 };

		float Multiplier = 1.0f;
	};
	class Scene
	{
	public:
		Scene(const std::string& debugName = "Scene");
		~Scene();

		void Init();


		void OnUpdate();
		void OnRenderRuntime();
		void OnRenderEditor(const EditorCamera& editorCamera);
		void OnEvent(Event& e);


		// Runtime
		void OnRuntimeStart();
		void OnRuntimeStop();


		/*void SetCamera(const Camera& camera) { m_Camera = camera; }
		Camera& GetCamera() { return m_Camera; }*/

		float GetExposure() const { return m_Exposure; }
		float& GetExposure() { return m_Exposure; }

		float GetSkyboxLod() const { return m_SkyboxLod; }
		float& GetSkyboxLod() { return m_SkyboxLod; }
		
		//void SetSkyboxMaterial(const Ref<MaterialInstance>& skybox) { m_SkyboxMaterial = skybox; }
		void SetSkyboxMaterial(const Ref<Material>& skybox) { m_SkyboxMaterial = skybox; }

		void SetEnvironment(const Environment& environment) { 

			m_Environment = environment; 
			//m_SkyboxMaterial->Set("u_Texture", environment.IrradianceMap);
			m_SkyboxMaterial->Set("u_Texture", environment.RadianceMap);
		}
		const Environment& GetEnvironment() const { return m_Environment; }

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

		//Entity EntityConstructor(const entt::entity& enttEntity);

		void OnViewportResize(uint32_t width, uint32_t height);

		const EntityMap& GetEntityMap() const { return m_EntityIDMap; }
		void CopyTo(Ref<Scene>& target);

		UUID GetUUID() const { return m_SceneID; }

		static Ref<Scene> GetScene(UUID uuid);

		// Editor-specific
		void SetSelectedEntity(entt::entity entity) { m_SelectedEntity = entity; }


	private:

		UUID m_SceneID;
		//entt::entity m_SceneEntity;

		// cotnainer for the actual component data (IDs)
		entt::registry m_Registry;
		EntityMap m_EntityIDMap;


		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		std::string m_DebugName;
		//Camera m_Camera;

		Light m_Light{};
		float m_LightMultiplier = 0.3f;

		Environment m_Environment{};

		Ref<Material> m_SkyboxMaterial = nullptr;
		//Ref<MaterialInstance> m_SkyboxMaterial = nullptr;
		
		float m_SkyboxLod = 1.0f;

		float m_Exposure = 0.8f;
		bool m_IsPlaying = false;

		entt::entity m_SelectedEntity;



		friend class SceneRenderer;
		friend class Entity;
		friend class SceneSerializer;

		friend class SceneHierarchyPanel;
		friend class EditorLayer;
	};

}
