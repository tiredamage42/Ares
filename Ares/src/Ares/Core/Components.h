#pragma once

#include "Ares/Renderer/Texture.h"
#include "Ares/Renderer/Camera.h"
#include <glm/glm.hpp>
namespace Ares
{
    struct TagComponent
    {
        std::string Tag;
        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag)
            : Tag(tag) {}

    };

    struct TransformComponent
    {
        glm::mat4 Transform{ 1.0f };

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        
        operator glm::mat4& () { return Transform; }
        operator const glm::mat4& () const { return Transform; }
    };

    struct SpriteRendererComponent
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
    };

    struct CameraComponent
    {
        Camera Camera;
        bool Primary = true;
        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
        CameraComponent(const glm::mat4& proj)
            : Camera(proj) {}
    };
        
}