#pragma once

#include "Hazel/Renderer/Buffer.h"
#include <glm/gtc/type_ptr.hpp>

namespace Hazel {
    struct MeshVertex
    {
        static BufferLayout GetLayout()
        {
            return {
                {ShaderDataType::Float3, "a_Positions"},
                {ShaderDataType::Float4, "a_Color"    },
                {ShaderDataType::Float3, "a_TexCoord" },
                {ShaderDataType::Float3, "a_Normal"   }
            };
        }

        glm::vec3 m_Position;
        glm::vec4 m_Color;
        glm::vec3 m_TexCoord;
        glm::vec3 m_Normal;

        int EntityId;
    };
}