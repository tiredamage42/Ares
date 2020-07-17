#pragma once

#include "Ares/Scene/Entity.h"
#include "Ares/Renderer/Camera.h"
namespace Ares
{
	class Scene
	{
	public:
		Scene(const std::string& debugName);
		~Scene();
		void Init();
		void OnUpdate();
		void SetCamera(const Camera& camera);
		void LoadEnvironmentMap(const std::string& filepath);
		void SetEnvironmentMaps(const Ref<TextureCube>& environmentRadianceMap, const Ref<TextureCube>& environmentIrradianceMap);
		void SetSkybox(const Ref<TextureCube>& skybox);
		void AddEntity(Entity* entity);
		Entity* CreateEntity();

	private:

		Ref<TextureCube> m_EnvironmentRadianceMap; 
		Ref<TextureCube> m_EnvironmentIrradianceMap;
		Ref<TextureCube> m_SkyboxTexture;
		Ref<Material> m_SkyboxMaterial;

		Camera m_Camera;
		std::string m_DebugName;
		std::vector<Entity> m_Entities;
	};
}