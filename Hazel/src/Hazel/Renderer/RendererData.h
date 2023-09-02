#pragma once

#include "Hazel/Renderer/RendererDataStruct.h"


namespace Hazel {

    struct RendererData
    {
        MeshData m_MeshData;

        void MeshDataInit();
    };
    static RendererData s_RendererData;

}


namespace Hazel {
    struct RendererConfig
    {
        static constexpr uint32_t MaxTriangles = 20000;
        static constexpr uint32_t MaxVertices = MaxTriangles * 3;
        static constexpr uint32_t MaxIndices = MaxTriangles * 3;
        static constexpr uint32_t MaxTextureSlots = 32; // TODO: RenderCaps
    };;

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