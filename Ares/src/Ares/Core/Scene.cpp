
#include "AresPCH.h"
#include "Ares/Core/Scene.h"
#include "Ares/Core/Components.h"
#include "Ares/Renderer/Renderer2D.h"
#include "Ares/Core/Entity.h"

namespace Ares
{
    Scene::Scene()
    {

    }
    Scene::~Scene()
    {

    }
    Entity Scene::CreateEntity(const std::string& name)
    {
        // entt::entity = uint32_t
        Entity entity = { m_Registry.create(), this };
        entity.AddComponent<TagComponent>(name.empty() ? "Entity" : name);
        entity.AddComponent<TransformComponent>();
        return entity;
    }
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

            Renderer2D::BeginScene(mainCamera->GetProjectionMatrix(), *cameraTransform);

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
