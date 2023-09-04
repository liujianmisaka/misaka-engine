#include "hzpch.h"
#include "RendererData.h"

Hazel::RendererData::RendererData(MeshData data)
{
    m_VertexArray = VertexArray::Create();
    m_VertexBuffer = VertexBuffer::Create(RendererConfig::MaxIndices * sizeof(MeshVertex));
    m_VertexBuffer->SetLayout(MeshVertex::GetLayout());
    m_VertexArray->AddVertexBuffer(m_VertexBuffer);
    m_VertexBufferBase = new MeshVertex[RendererConfig::MaxVertices];
    Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(data.m_Indices.data(), RendererConfig::MaxIndices);
    m_VertexArray->SetIndexBuffer(indexBuffer);
    m_Shader = Shader::Create("assets/shaders/Renderer2D_Mesh.glsl");
}