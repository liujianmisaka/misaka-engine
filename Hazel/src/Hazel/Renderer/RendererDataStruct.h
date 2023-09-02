#pragma once

#include "Hazel/Renderer/VertexArray.h"
#include "Hazel/Renderer/Shader.h"
#include "Hazel/Vertex/MeshVertex.h"

namespace Hazel {

    struct MeshData
    {
        Ref<VertexArray> MeshVertexArray;
        Ref<VertexBuffer> MeshVertexBuffer;
        Ref<Shader> MeshShader;
        uint32_t MeshIndexCount = 0;
        MeshVertex* MeshVertexBufferBase = nullptr;
        MeshVertex* MeshVertexBufferPtr = nullptr;
    };
}