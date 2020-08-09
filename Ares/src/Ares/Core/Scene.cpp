
#include "AresPCH.h"
#include "Ares/Core/Scene.h"
#include "Ares/Core/Components.h"
#include "Ares/Renderer/Renderer2D.h"
#include "Ares/Renderer/SceneRenderer.h"
#include "Ares/Core/Entity.h"

namespace Ares
{

    std::unordered_map<uint32_t, Scene*> s_ActiveScenes;

    struct SceneComponent
    {
        uint32_t SceneID;
    };

    static uint32_t s_SceneIDCounter = 0;

    void OnTransformConstruct(entt::registry& registry, entt::entity entity)
    {
        // HZ_CORE_TRACE("Transform Component constructed!");
    }

    
    Scene::Scene(const std::string& debugName)
        : m_DebugName(debugName), m_SceneID(++s_SceneIDCounter)
    {

        m_Registry.on_construct<TransformComponent>().connect<&OnTransformConstruct>();

        //m_SceneEntity = m_Registry.create();
        //m_Registry.emplace<SceneComponent>(m_SceneEntity, m_SceneID);

        s_ActiveScenes[m_SceneID] = this;

        Init();
    }
    Scene::~Scene()
    {
        m_Registry.clear();

        s_ActiveScenes.erase(m_SceneID);
    }
    void Scene::Init()
    {
        /*auto skyboxShader = Shader::Create("assets/shaders/Skybox.glsl");
        m_SkyboxMaterial = MaterialInstance::Create(Material::Create(skyboxShader));
        m_SkyboxMaterial->SetFlag(MaterialFlag::DepthTest, false);*/
    }

    Entity Scene::CreateEntity(const std::string& name)
    {
        // entt::entity = uint32_t
        Entity entity = { m_Registry.create(), this };
        entity.AddComponent<TagComponent>(name.empty() ? "Entity" : name);
        entity.AddComponent<TransformComponent>();
        return entity;
    }
    void Scene::DestroyEntity(Entity entity)
    {
        m_Registry.destroy(entity.m_EntityHandle);
    }

    /*Entity Scene::EntityConstructor(const entt::entity& enttEntity)
    {
        return { enttEntity, this };
    }*/
    void Scene::OnViewportResize(uint32_t width, uint32_t height)
    {
        m_ViewportWidth = width;
        m_ViewportHeight = height;

        // resize our non fixed aspect ratio cameras

        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view)
        {
            auto& cameraComponent = view.get<CameraComponent>(entity);

            if (!cameraComponent.FixedAspectRatio)
            {
                cameraComponent.Camera.SetViewportSize(width, height);
            }
        }
        
    }
    void Scene::OnUpdate()
    {
        // 3d
        if (true)
        {
            Camera* camera = nullptr;
            {
                auto view = m_Registry.view<CameraComponent>();
                for (auto entity : view)
                {
                    auto& comp = view.get<CameraComponent>(entity);
                    camera = &comp.Camera;
                    break;
                }
            }


            if (camera)
            {
                camera->Update();

                // Update all entities
                {
                    
                }
            }


            //m_Camera.Update();


            // Update all entities

            /*auto view = m_Registry.view<MeshRendererComponent>();
            for (auto entity : view)
            {
                auto& meshComponent = view.get<MeshRendererComponent>(entity);
                if (meshComponent.Mesh)
                {
                    meshComponent.Mesh->OnUpdate();
                }    
            }*/

            /*m_SkyboxMaterial->Set("u_TextureLod", GetSkyboxLod());
            */




            SceneRenderer::BeginScene(this, *camera);

            auto group = m_Registry.group<MeshRendererComponent>(entt::get<TransformComponent>);
            // Render entities
            for (auto entity : group)
            {
                auto [transformComponent, meshComponent] = group.get<TransformComponent, MeshRendererComponent>(entity);
                if (meshComponent.Mesh)
                {
                    meshComponent.Mesh->OnUpdate();

                    // TODO: Should we render (logically)
                    SceneRenderer::SubmitMesh(meshComponent.Mesh, transformComponent, {});
                }
            }
            //for (auto entity : view)
            //{
            //    Entity entityS = { entity, this };
            //    // TODO: Should we render (logically)
            //    SceneRenderer::SubmitEntity(entityS);
            //}
            
            
            SceneRenderer::EndScene();
        }
        else
        {

            // Render 2D
            Camera* mainCamera = nullptr;
            glm::mat4* cameraTransform = nullptr;
            {
                auto view = m_Registry.view<TransformComponent, CameraComponent>();
                for (auto entity : view)
                {
                    auto& [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

                    if (camera.Primary)
                    {
                        mainCamera = &camera.Camera;
                        cameraTransform = &transform.Transform;
                        break;
                    }
                }
            }

            if (mainCamera)
            {

                glm::mat4 viewProj = mainCamera->GetProjectionMatrix() * glm::inverse(*cameraTransform);

                Renderer2D::BeginScene(viewProj, true);
                //Renderer2D::BeginScene(mainCamera->GetProjectionMatrix(), *cameraTransform);

                auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        
                for (auto entity : group)
                {
                    //ARES_CORE_LOG("Rendering sprite");

                    auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
            
                    Renderer2D::DrawQuad(transform, sprite.Texture, sprite.Tiling, sprite.Offset, sprite.Color);
                }

                Renderer2D::EndScene();

            }
        }



    }

    void Scene::OnEvent(Event& e)
    {
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view)
        {
            auto& comp = view.get<CameraComponent>(entity);
            comp.Camera.OnEvent(e);
            break;
        }
        //m_Camera.OnEvent(e);
    }
















	Environment Environment::Load(const std::string& filepath)
	{
		auto [radiance, irradiance] = SceneRenderer::CreateEnvironmentMap(filepath);
		return { radiance, irradiance };
	}
}
