#include "hzpch.h"
#include "Hazel/Renderer/RendererData.h"
#include "Hazel/Vertex/MeshVertex.h"

void Hazel::RendererData::MeshDataInit()
{
    m_MeshData.MeshVertexArray = VertexArray::Create();
    m_MeshData.MeshVertexBuffer = VertexBuffer::Create(RendererConfig::MaxIndices * sizeof(MeshVertex));
    m_MeshData.MeshVertexBuffer->SetLayout(MeshVertex::GetLayout());
    m_MeshData.MeshVertexArray->AddVertexBuffer(m_MeshData.MeshVertexBuffer);
    m_MeshData.MeshVertexBufferBase = new MeshVertex[RendererConfig::MaxVertices];
    auto* meshIndices = new uint32_t[RendererConfig::MaxIndices];

    uint32_t meshOffset = 0;
    for (uint32_t i = 0; i < RendererConfig::MaxIndices; i += 3) {
        meshIndices[i + 0] = meshOffset + 0;
        meshIndices[i + 1] = meshOffset + 1;
        meshIndices[i + 2] = meshOffset + 2;

        meshOffset += 3;
    }

    Ref<IndexBuffer> meshIB = IndexBuffer::Create(meshIndices, RendererConfig::MaxIndices);
    m_MeshData.MeshVertexArray->SetIndexBuffer(meshIB);
    delete[] meshIndices;

    m_MeshData.MeshShader = Shader::Create("assets/shaders/Renderer2D_Mesh.glsl");
}
