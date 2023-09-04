#pragma once

#include "Hazel/Renderer/VertexArray.h"
#include "Hazel/Renderer/Shader.h"
#include "Hazel/Vertex/MeshVertex.h"

namespace Hazel {

    struct MeshData
    {
        std::vector<MeshVertex> m_Vertices;
        std::vector<uint32_t> m_Indices;
    };

    class RendererData
    {
    public:
        RendererData(MeshData data);

        Ref<VertexArray> m_VertexArray;
        Ref<VertexBuffer> m_VertexBuffer;
        Ref<Shader> m_Shader;
        uint32_t m_IndexCount = 0;
        MeshVertex* m_VertexBufferBase = nullptr;
        MeshVertex* m_VertexBufferPtr = nullptr;
    };
}

namespace Hazel {
    struct RendererConfig
    {
        static constexpr uint32_t MaxTriangles = 20000;
        static constexpr uint32_t MaxVertices = MaxTriangles * 3;
        static constexpr uint32_t MaxIndices = MaxTriangles * 3;
        static constexpr uint32_t MaxTextureSlots = 32; // TODO: RenderCaps
    };

    // Stats
    struct Statistics
    {
        uint32_t DrawCalls = 0;
        uint32_t QuadCount = 0;

        uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
        uint32_t GetTotalIndexCount() const { return QuadCount * 6; }

        static void ResetStats();
        static Statistics GetStats();
    };
    static Statistics s_statistics;
}