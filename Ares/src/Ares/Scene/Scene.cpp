
#include "AresPCH.h"
#include "Ares/Scene/Scene.h"

#include "Ares/Renderer/SceneRenderer.h"
namespace Ares
{
	Scene::Scene(const std::string& debugName)
		: m_DebugName(debugName)
	{
		Init();
	}
	Scene::~Scene()
	{
		for (Entity* entity : m_Entities)
			delete entity;
	}
	void Scene::Init()
	{
		auto skyboxShader = Shader::Create("Assets/Shaders/quad.glsl");
		m_SkyboxMaterial = MaterialInstance::Create(Material::Create(skyboxShader));
		m_SkyboxMaterial->SetFlag(MaterialFlag::DepthTest, false);
	}
	void Scene::OnUpdate()
	{
		m_Camera.Update();

		// update all entities
		for (auto entity : m_Entities)
		{
			auto mesh = entity->GetMesh();
			if (mesh)
				mesh->OnUpdate();
		}
		SceneRenderer::BeginScene(this);

		//render entities
		for (auto entity : m_Entities)
		{
			SceneRenderer::SubmitEntity(entity);
		}

		SceneRenderer::EndScene();

	}
	void Scene::SetCamera(const Camera& camera)
	{
		m_Camera = camera;
	}
	void Scene::LoadEnvironmentMap(const std::string& filepath)
	{
		SceneRenderer::CreateEnvironmentMap(filepath);
	}
	void Scene::SetEnvironmentMaps(const Ref<TextureCube>& environmentRadianceMap, const Ref<TextureCube>& environmentIrradianceMap)
	{
		m_EnvironmentRadianceMap = environmentRadianceMap;
		m_EnvironmentIrradianceMap = environmentIrradianceMap;
	}
	void Scene::SetSkybox(const Ref<TextureCube>& skybox)
	{
		m_SkyboxTexture = skybox;
		m_SkyboxMaterial->Set("u_Texture", skybox);
	}
	void Scene::AddEntity(Entity* entity)
	{
		m_Entities.push_back(entity);
	}
	Entity* Scene::CreateEntity()
	{
		Entity* entity = new Entity();
		AddEntity(entity);
		return entity;
	}
}