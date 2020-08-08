#pragma once

#include "entt.hpp"
#include "Ares/Renderer/Camera.h"
#include "Ares/Renderer/Material.h"
namespace Ares
{
	class Entity;

	struct Environment
	{
		Ref<TextureCube> RadianceMap;
		Ref<TextureCube> IrradianceMap;

		static Environment Load(const std::string& filepath);
	};
	struct Light
	{
		glm::vec3 Direction;
		glm::vec3 Radiance;

		float Multiplier = 1.0f;
	};
	class Scene
	{
	public:
		Scene(const std::string& debugName = "Scene");
		~Scene();

		void OnUpdate();

		void SetCamera(const Camera& camera) { m_Camera = camera; }
		Camera& GetCamera() { return m_Camera; }

		float GetExposure() const { return m_Exposure; }
		float& GetExposure() { return m_Exposure; }

		float GetSkyboxLod() const { return m_SkyboxLod; }
		float& GetSkyboxLod() { return m_SkyboxLod; }
		
		void SetSkyboxMaterial(const Ref<MaterialInstance>& skybox) { m_SkyboxMaterial = skybox; }
		void SetEnvironment(const Environment& environment) { 
			m_Environment = environment; 
			m_SkyboxMaterial->Set("u_Texture", environment.RadianceMap);
		}
		Light& GetLight() { return m_Light; }



		void OnEvent(Event& e);

		Entity CreateEntity(const std::string& name = std::string());
		Entity EntityConstructor(const entt::entity& enttEntity);

		void OnViewportResize(uint32_t width, uint32_t height);



	private:
		// cotnainer for the actual component data (IDs)
		entt::registry m_Registry;

		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		std::string m_DebugName;
		Camera m_Camera;

		Light m_Light;
		float m_LightMultiplier = 0.3f;

		Environment m_Environment;
		Ref<MaterialInstance> m_SkyboxMaterial;
		float m_SkyboxLod = 1.0f;

		float m_Exposure = 0.8f;



		friend class SceneRenderer;
		friend class Entity;

		friend class SceneHierarchyPanel;
		friend class EditorLayer;
	};

}
