#pragma once

#include "Hazel/Renderer/Buffer.h"
#include <glm/glm.hpp>

namespace Hazel {
    struct MeshVertex
    {
        static BufferLayout GetLayout()
        {
            return {
                {ShaderDataType::Float3, "a_Position"},
                {ShaderDataType::Float3, "a_Normal"   },
                {ShaderDataType::Float2, "a_TexCoord" },
                {ShaderDataType::Float3, "a_Tangent"},
                {ShaderDataType::Float3, "a_Bitangent"}
            };
        }

        glm::vec3 m_Position = {0.0, 0.0, 0.0};
        glm::vec3 m_Normal = { 0.0, 0.0, 0.0 };
        glm::vec2 m_TexCoord = { 0.0, 0.0 };
        glm::vec3 m_Tangent = { 0.0, 0.0, 0.0 };
        glm::vec3 m_Bitangent = { 0.0, 0.0, 0.0 };

        int EntityId;
    };
}