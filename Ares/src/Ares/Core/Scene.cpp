
#include "AresPCH.h"
#include "Ares/Core/Scene.h"
#include "Ares/Core/Components.h"
#include "Ares/Renderer/Renderer2D.h"
#include "Ares/Renderer/SceneRenderer.h"
#include "Ares/Core/Entity.h"

#include "Ares/Math/Math.h"
/*
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
*/

namespace Ares
{

    std::unordered_map<UUID, Scene*> s_ActiveScenes;

    struct SceneComponent
    {
        uint32_t SceneID;
    };

    Scene::Scene(const std::string& debugName)
        : m_DebugName(debugName)
    {
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

    }

    static std::tuple<glm::vec3, glm::quat, glm::vec3> GetTransformDecomposition(const glm::mat4& transform)
    {
        glm::vec3 scale, translation, skew;
        glm::vec4 perspective;
        glm::quat orientation;
        glm::decompose(transform, scale, orientation, translation, skew, perspective);

        return { translation, orientation, scale };
    }

    Entity Scene::CreateEntity(const std::string& name)
    {
        // entt::entity = uint32_t
        Entity entity = { m_Registry.create(), this };

        auto* idComponent = entity.AddComponent<IDComponent>();
        idComponent->ID = {};
        entity.AddComponent<TagComponent>(name.empty() ? "Entity" : name);

        entity.AddComponent<TransformComponent>(glm::mat4(1.0f));
        
        m_EntityIDMap[idComponent->ID] = entity;

        return entity;
    }
    Entity Scene::CreateEntityWithID(UUID uuid, const std::string& name, bool runtimeMap)
    {
        ARES_CORE_ASSERT(m_EntityIDMap.find(uuid) == m_EntityIDMap.end(), "");
        
        auto entity = Entity{ m_Registry.create(), this };
        auto* idComponent = entity.AddComponent<IDComponent>();
        idComponent->ID = uuid;

        entity.AddComponent<TagComponent>(name.empty() ? "Entity" : name);
        entity.AddComponent<TransformComponent>(glm::mat4(1.0f));
        
        m_EntityIDMap[uuid] = entity;
        return entity;
    }
    void Scene::DestroyEntity(Entity entity)
    {
        m_Registry.destroy(entity.m_EntityHandle);
    }

    void Scene::OnViewportResize(uint32_t width, uint32_t height)
    {
        m_ViewportWidth = width;
        m_ViewportHeight = height;


        // 2D:
        // resize our non fixed aspect ratio cameras
        /*auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view)
        {
            auto& cameraComponent = view.get<CameraComponent>(entity);

            if (!cameraComponent.FixedAspectRatio)
            {
                cameraComponent.Camera.SetViewportSize(width, height);
            }
        }*/
        
    }
    void Scene::ForEachEntity(std::function<void(const Entity&)> foreach)
    {
        m_Registry.each([&](auto entity)
        {
            Entity e(entity, this);
            foreach(e);
        });
    }
    void Scene::OnUpdate()
    {
        // 3d
        if (true)
        {
            
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

    void Scene::OnRenderRuntime()
    {
        Entity cameraEntity = GetMainCameraEntity();
        if (!cameraEntity)
            return;

        glm::mat4 cameraViewMatrix = glm::inverse(cameraEntity.GetComponent<TransformComponent>()->Transform);
        SceneCamera& camera = cameraEntity.GetComponent<CameraComponent>()->Camera;
        
        // needs to happen every render???
        camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);

        auto group = m_Registry.group<MeshRendererComponent>(entt::get<TransformComponent>);
        SceneRenderer::BeginScene(this, { camera, cameraViewMatrix });
        for (auto entity : group)
        {
            auto [transformComponent, meshComponent] = group.get<TransformComponent, MeshRendererComponent>(entity);
            if (meshComponent.Mesh)
            {
                meshComponent.Mesh->OnUpdate();

                // TODO: Should we render (logically)
                SceneRenderer::SubmitMesh(meshComponent.Mesh, transformComponent, meshComponent.Materials, false);
            }
        }
        SceneRenderer::EndScene();


        // RENDER SPRITES HERE
    }

    void Scene::OnRenderEditor(const EditorCamera& editorCamera, const Entity& selectedEntity)
    {
        auto group = m_Registry.group<MeshRendererComponent>(entt::get<TransformComponent>);
        SceneRenderer::BeginScene(this, { editorCamera, editorCamera.GetViewMatrix() });
        for (auto entity : group)
        {
            auto [transformComponent, meshComponent] = group.get<TransformComponent, MeshRendererComponent>(entity);
            if (meshComponent.Mesh)
            {
                meshComponent.Mesh->OnUpdate();

                // TODO: Should we render (logically)
                SceneRenderer::SubmitMesh(meshComponent.Mesh, transformComponent, meshComponent.Materials, selectedEntity == entity);
            }
        }
        SceneRenderer::EndScene();

        // RENDER SPRITES HERE
    }

    void Scene::OnEvent(Event& e)
    {
        
    }

    void Scene::OnRuntimeStart()
    {
        m_IsPlaying = true;
    }

    void Scene::OnRuntimeStop()
    {
        m_IsPlaying = false;
    }

    Entity Scene::GetMainCameraEntity()
    {
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view)
        {
            auto& comp = view.get<CameraComponent>(entity);
            if (comp.Primary)
                return { entity, this };
        }
        return {};
    }

    template<typename T>
    static void CopyComponent(entt::registry& dstRegistry, entt::registry& srcRegistry, const std::unordered_map<UUID, entt::entity>& enttMap)
    {
        auto components = srcRegistry.view<T>();
        for (auto srcEntity : components)
        {
            entt::entity destEntity = enttMap.at(srcRegistry.get<IDComponent>(srcEntity).ID);

            auto& srcComponent = srcRegistry.get<T>(srcEntity);
            auto& destComponent = dstRegistry.emplace_or_replace<T>(destEntity, srcComponent);
        }
    }

    template<typename T>
    static void CopyComponentIfExists(entt::entity dst, entt::entity src, entt::registry& registry)
    {
        if (registry.has<T>(src))
        {
            auto& srcComponent = registry.get<T>(src);
            registry.emplace_or_replace<T>(dst, srcComponent);
        }
    }

    void Scene::DuplicateEntity(Entity entity)
    {
        Entity newEntity;
        if (entity.HasComponent<TagComponent>())
            newEntity = CreateEntity(entity.GetComponent<TagComponent>()->Tag);
        else
            newEntity = CreateEntity();

        CopyComponentIfExists<TransformComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_Registry);
        CopyComponentIfExists<MeshRendererComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_Registry);
        CopyComponentIfExists<CameraComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_Registry);
        CopyComponentIfExists<SpriteRendererComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_Registry);
    }

    // Copy to runtime
    void Scene::CopyTo(Ref<Scene>& target)
    {
        // Environment
        target->m_Light = m_Light;
        target->m_Environment = m_Environment;
        target->m_SkyboxMaterial = m_SkyboxMaterial;
        target->m_Exposure = m_Exposure;

        std::unordered_map<UUID, entt::entity> enttMap;
        auto idComponents = m_Registry.view<IDComponent>();
        for (auto entity : idComponents)
        {
            auto uuid = m_Registry.get<IDComponent>(entity).ID;
            Entity e = target->CreateEntityWithID(uuid, "", true);
            enttMap[uuid] = e.m_EntityHandle;
        }

        CopyComponent<TagComponent>(target->m_Registry, m_Registry, enttMap);
        CopyComponent<TransformComponent>(target->m_Registry, m_Registry, enttMap);
        CopyComponent<MeshRendererComponent>(target->m_Registry, m_Registry, enttMap);
        CopyComponent<CameraComponent>(target->m_Registry, m_Registry, enttMap);
        CopyComponent<SpriteRendererComponent>(target->m_Registry, m_Registry, enttMap);
    }

    Ref<Scene> Scene::GetScene(UUID uuid)
    {
        Ref<Scene> r = nullptr;
        if (s_ActiveScenes.find(uuid) != s_ActiveScenes.end())
        {
            r.reset(s_ActiveScenes.at(uuid));
            return r;
        }
        return nullptr;
    }



	Environment Environment::Load(const std::string& filepath)
	{
		auto [radiance, irradiance] = SceneRenderer::CreateEnvironmentMap(filepath);
		return { filepath, radiance, irradiance };
	}
}
