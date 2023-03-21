#pragma once

#include "Hazel/Renderer/VertexArray.h"

namespace Hazel {
	class OpenGLVertexArray : public VertexArray {
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

		virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const override { return m_Vertexbuffers; }
		virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const override  { return m_Indexbuffer; }
	private:
		uint32_t m_RendererID;
		std::vector<std::shared_ptr<VertexBuffer> > m_Vertexbuffers;
		std::shared_ptr<IndexBuffer> m_Indexbuffer;
	};
}