#include "hzpch.h"
#include "Hazel/Renderer/Renderer3D.h"
#include "Hazel/Renderer/RenderCommand.h"
#include "Hazel/Renderer/RendererData.h"
#include "Hazel/Model/Model.h"

namespace Hazel
{
    static std::vector<RendererData> s_RendererMeshData;


    void Renderer3D::Init()
    {
        HZ_PROFILE_FUNCTION();

        Model model("assets/stylized-popcorn-machine-lowpoly/source/SM_Popcorn Machine.fbx");
        std::vector<MeshData> meshdata = model.GetData();
        for (const MeshData& data : meshdata)
        {
            RendererData renderData(data);
            s_RendererMeshData.push_back(renderData);
        }
    }

    void Renderer3D::Shutdown()
    {
        HZ_PROFILE_FUNCTION();

        for (const RendererData& rendererData : s_RendererMeshData)
        {
            delete[] rendererData.m_VertexBufferBase;
        }
    }

    void Renderer3D::Flush()
    {
        for (const RendererData& rendererData : s_RendererMeshData)
        {
            if (rendererData.m_IndexCount)
            {
                uint32_t dataSize = (uint32_t)((uint8_t*)rendererData.m_VertexBufferPtr - (uint8_t*)rendererData.m_VertexBufferBase);
                rendererData.m_VertexBuffer->SetData(rendererData.m_VertexBufferBase, dataSize);

                rendererData.m_Shader->Bind();
                RenderCommand::DrawIndexed(rendererData.m_VertexArray, rendererData.m_IndexCount);
                s_statistics.DrawCalls++;
            }
        }
    }

    void Renderer3D::DrawMesh(const glm::mat4& transform, const std::vector<MeshVertex>& vertices)
    {
        for (RendererData& rendererData : s_RendererMeshData)
        {
            for (const auto& vertex : vertices)
            {
                glm::vec4 position = { vertex.m_Position.x, vertex.m_Position.y, vertex.m_Position.z, 1.0f };
                position = transform * position;

                rendererData.m_VertexBufferPtr->m_Position = { position.x / position.w, position.y / position.w, position.z / position.w };
                rendererData.m_VertexBufferPtr->m_Normal = vertex.m_Normal;
                rendererData.m_VertexBufferPtr->m_TexCoord = vertex.m_TexCoord;
                rendererData.m_VertexBufferPtr->m_Tangent = vertex.m_Tangent;
                rendererData.m_VertexBufferPtr->m_Bitangent = vertex.m_Bitangent;
                rendererData.m_VertexBufferPtr++;

                rendererData.m_IndexCount += 1;
            }
        }
    }

    void Renderer3D::StartBatch()
    {
        for (RendererData& rendererData : s_RendererMeshData)
        {
            rendererData.m_IndexCount = 0;
            rendererData.m_VertexBufferPtr = rendererData.m_VertexBufferBase;
        }
    }

    void Renderer3D::NextBatch()
    {
        Flush();
        StartBatch();
    }

}
