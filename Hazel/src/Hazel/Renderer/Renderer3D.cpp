#include "hzpch.h"
#include "Hazel/Renderer/Renderer3D.h"
#include "Hazel/Renderer/RenderCommand.h"

void Hazel::Renderer3D::Init()
{
    HZ_PROFILE_FUNCTION();

    s_RendererData.MeshDataInit();
}

void Hazel::Renderer3D::Shutdown()
{
    HZ_PROFILE_FUNCTION();

    delete[] s_RendererData.m_MeshData.MeshVertexBufferBase;
}

void Hazel::Renderer3D::Flush()
{
    if (s_RendererData.m_MeshData.MeshIndexCount)
    {
        uint32_t dataSize = (uint32_t)((uint8_t*)s_RendererData.m_MeshData.MeshVertexBufferPtr - (uint8_t*)s_RendererData.m_MeshData.MeshVertexBufferBase);
        s_RendererData.m_MeshData.MeshVertexBuffer->SetData(s_RendererData.m_MeshData.MeshVertexBufferBase, dataSize);

        s_RendererData.m_MeshData.MeshShader->Bind();
        RenderCommand::DrawIndexed(s_RendererData.m_MeshData.MeshVertexArray, s_RendererData.m_MeshData.MeshIndexCount);
        s_statistics.DrawCalls++;
    }
}

void Hazel::Renderer3D::DrawMesh(const std::vector<MeshVertex>& meshes)
{
    for (const auto mesh : meshes)
    {
        s_RendererData.m_MeshData.MeshVertexBufferPtr->m_Position = mesh.m_Position;
        s_RendererData.m_MeshData.MeshVertexBufferPtr->m_Normal = mesh.m_Normal;
        s_RendererData.m_MeshData.MeshVertexBufferPtr->m_Color = mesh.m_Color;
        s_RendererData.m_MeshData.MeshVertexBufferPtr->m_TexCoord = mesh.m_TexCoord;
        s_RendererData.m_MeshData.MeshVertexBufferPtr++;

        s_RendererData.m_MeshData.MeshIndexCount += 1;
    }
}

void Hazel::Renderer3D::StartBatch()
{
    s_RendererData.m_MeshData.MeshIndexCount = 0;
    s_RendererData.m_MeshData.MeshVertexBufferPtr = s_RendererData.m_MeshData.MeshVertexBufferBase;
}

void Hazel::Renderer3D::NextBatch()
{
}
