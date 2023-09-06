#pragma once

#include <glm/glm.hpp>

namespace Hazel
{
    
    struct QuadVertex {
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;
        float TexIndex;
        float TilingFactor;

        // Editor only
        int EntityId;
    };

    struct CircleVertex {
        glm::vec3 WorldPosition;
        glm::vec3 LocalPosition;
        glm::vec4 Color;
        float Thickness;
        float Fade;

        // Editor only
        int EntityId;
    };

    struct LineVertex {
        glm::vec3 Position;
        glm::vec4 Color;

        // Editor only
        int EntityId;
    };

    struct MeshVertex
    {
        glm::vec3 Position;
        glm::vec3 Color;
        glm::vec3 Normal;
        glm::vec2 TexCoord;
        glm::vec3 Tangent;
        glm::vec3 Bitangent;
    };
}
