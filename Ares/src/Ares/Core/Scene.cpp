
#include "AresPCH.h"
#include "Ares/Core/Scene.h"
#include "Ares/Core/Components.h"
#include "Ares/Renderer/Renderer2D.h"
#include "Ares/Core/Entity.h"

//#include <glm/glm.hpp>
namespace Ares
{
    Scene::Scene()
    {

        //struct TransformComponent
        //{
        //    glm::mat4 Transform;

        //    TransformComponent() = default;
        //    TransformComponent(const TransformComponent&) = default;
        //    TransformComponent(const glm::mat4& transform)
        //        : Transform(transform) {}
        //};

        //// entt::entity = uint32_t
        //entt::entity entity = m_Registry.create();
        //m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));

        //// get a reference to the transform component
        //TransformComponent& transform = m_Registry.get<TransformComponent>(entity);

    }
    Scene::~Scene()
    {
    }
    Entity Scene::CreateEntity(const std::string& name)
    {
        // entt::entity = uint32_t
        Entity entity = { m_Registry.create(), this };

        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;

        entity.AddComponent<TransformComponent>();

        return entity;
    }
    void Scene::OnUpdate()
    {
        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        //auto group = m_Registry.group<TransformComponent>();

        for (auto entity : group)
        {
            auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
            
            Renderer2D::DrawQuad(transform, nullptr, 1.0f, sprite.Color);
        }
        /*
        */
    }
}
