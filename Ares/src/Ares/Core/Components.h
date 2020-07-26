#pragma once

#include "Ares/Renderer/Texture.h"
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
        /*TransformComponent(const glm::mat4& transform)
            : Transform(transform) {}*/

        operator glm::mat4& () { return Transform; }
        operator const glm::mat4& () const { return Transform; }
    };

    struct SpriteRendererComponent
    {
        glm::vec4 Color{ 1.0f,1.0f,1.0f,1.0f };
        glm::vec2 Tiling{ 1.0f };
        glm::vec2 Offset{ 0.0f };
        Ref<Texture2D> Texture = nullptr;

        SpriteRendererComponent() = default;
        SpriteRendererComponent(const SpriteRendererComponent&) = default;
        /*SpriteRendererComponent(const glm::vec4& color)
            : Color(color) {}*/
    };
}