#pragma once

#include "Ares/Core/UUID.h"
#include "Ares/Renderer/Texture.h"
#include "Ares/Renderer/SceneCamera.h"
#include "Ares/Renderer/Animator.h"
#include "Ares/Renderer/Mesh.h"
//#include <glm/glm.hpp>
#include "Ares/Math/Math.h"
//#include "Ares/Core/Entity.h"

namespace Ares
{
    //class Entity;

    struct Component
    {
        virtual void OnDrawImGui();
    };
    /*
    struct IDComponent
    {
        UUID ID = 0;
    };

    */

    // basic entity information
    struct EntityComponent
    {
        bool Enabled;
        UUID ID = 0;
        std::string Name;
        std::string Tag;
        EntityComponent() = default;
        EntityComponent(const EntityComponent&) = default;
    };

    /*
    struct TagComponent
    {
        std::string Tag;
        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag) : Tag(tag) {}

        operator std::string& () { return Tag; }
        operator const std::string& () const { return Tag; }
    };
    */

    struct TransformComponent
    {
        TransformComponent* Parent = nullptr;
        std::vector<TransformComponent*> Children;
        UUID Entity;
        glm::mat4 LocalTransform{ 1.0f };


        void SetParent(TransformComponent* parent)
        {
             
            if (Parent != parent && parent != this)
            {
                if (Parent)
                {
                    for (uint32_t i = 0; i < Parent->Children.size(); i++)
                    {
                        if (Parent->Children[i] == this)
                        {
                            Parent->Children.erase(Parent->Children.begin() + i);
                            break;
                        }
                    }
                }

                // make sure the transform is the same in world space
                // regardless of parent
                glm::mat4 worldTransform = GetWorldTransform();

                Parent = parent;
                Parent->Children.push_back(this);

                SetWorldTransform(worldTransform);
            }
        }

        const glm::mat4& GetLocalTransform() const { return LocalTransform; }
        glm::mat4 GetWorldTransform() const 
        {
            if (Parent)
            {
                return Parent->GetWorldTransform() * LocalTransform;
            }
            else
            {
                return LocalTransform;
            }
        };

        void SetLocalTransform(const glm::mat4& localTransform) { LocalTransform = localTransform; }
        void SetWorldTransform(const glm::mat4& worldTransform)
        {
            if (Parent)
            {
                // w = p * l;
                // l = w * inv(p);  
                LocalTransform = glm::inverse(Parent->GetWorldTransform()) * worldTransform;
            }
            else
            {
                LocalTransform = worldTransform;
            }
        }



        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        
        //operator glm::mat4& () { return Transform; }
        //operator const glm::mat4& () const { return Transform; }
    };

    struct SpriteRendererComponent : Component
    {
        glm::vec4 Color{ 1.0f,1.0f,1.0f,1.0f };
        glm::vec2 Tiling{ 1.0f };
        glm::vec2 Offset{ 0.0f };
        Ref<Texture2D> Texture = nullptr;

        void SetSpriteSheetCoords(const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize = glm::vec2(1.0f))
        {
            if (!Texture)
            {
                ARES_CORE_WARN("Cant Set Sprite Sheet Coordinates, No Texture Sprite Sheet Specified");
                return;
            }
            Texture2D::CalculateTilingAndOffsetForSubTexture(&Tiling, &Offset, Texture, coords, cellSize, spriteSize);
        }

        SpriteRendererComponent() = default;
        SpriteRendererComponent(const SpriteRendererComponent&) = default;

        //virtual void OnDrawImGui() override;
    };

    struct MeshRendererComponent : Component
    {
        Ref<Mesh> Mesh = nullptr;
        std::vector<Ref<Material>> Materials;

        MeshRendererComponent() = default;
        MeshRendererComponent(const MeshRendererComponent&) = default;

        virtual void OnDrawImGui() override;
    };

    struct AnimatorComponent : Component
    {
        //std::string Name;
        Animator Animator;
        AnimatorComponent() = default;
        AnimatorComponent(const AnimatorComponent&) = default;
        virtual void OnDrawImGui() override;
    };

    struct CameraComponent : Component
    {
        SceneCamera Camera;
        bool Primary = true;
        bool FixedAspectRatio = false;

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;

        operator SceneCamera& () { return Camera; }
        operator const SceneCamera& () const { return Camera; }

        virtual void OnDrawImGui() override;
    };
        
}