#pragma once

#include "Hazel/Renderer/Framebuffer.h"

namespace Hazel {
    class OpenGLFrameBuffer : public Framebuffer {
    public:
        OpenGLFrameBuffer(FramebufferSpecification spec);
        virtual  ~OpenGLFrameBuffer() override;

        void Invalidate();

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void Resize(uint32_t width, uint32_t height) override;
        virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

        virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override {
            HZ_CORE_ASSERT(index < m_ColorAttachments.size(), "index is bigger than color attachments size!")
            return m_ColorAttachments[index];
        }
        virtual uint32_t GetDepthAttachmentRendererID() const override { return m_DepthAttachment; }

        virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
    private:
        uint32_t m_RendererID = 0;
        FramebufferSpecification m_Specification;

        std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecification;
        FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

        std::vector<uint32_t> m_ColorAttachments;
        uint32_t m_DepthAttachment = 0;
    };
}