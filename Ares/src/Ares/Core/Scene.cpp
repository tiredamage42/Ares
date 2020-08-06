
#include "AresPCH.h"
#include "Ares/Core/Scene.h"
#include "Ares/Core/Components.h"
#include "Ares/Renderer/Renderer2D.h"
#include "Ares/Renderer/SceneRenderer.h"
#include "Ares/Core/Entity.h"

namespace Ares
{
    Scene::Scene(const std::string& debugName)
        : m_DebugName(debugName)
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
        // 3d
        if (true)
        {

            m_Camera.Update();


            // Update all entities

            auto view = m_Registry.view<MeshRendererComponent>();
            for (auto entity : view)
            {
                auto& meshComponent = view.get<MeshRendererComponent>(entity);
                if (meshComponent.Mesh)
                {
                    meshComponent.Mesh->OnUpdate();
                }    
            }

            /*m_SkyboxMaterial->Set("u_TextureLod", GetSkyboxLod());
            */
            SceneRenderer::BeginScene(this);

            // Render entities
            
            for (auto entity : view)
            {
                Entity entityS = { entity, this };
                // TODO: Should we render (logically)
                SceneRenderer::SubmitEntity(entityS);
            }
            
            
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


















	Environment Environment::Load(const std::string& filepath)
	{
		auto [radiance, irradiance] = SceneRenderer::CreateEnvironmentMap(filepath);
		return { radiance, irradiance };
	}
}
