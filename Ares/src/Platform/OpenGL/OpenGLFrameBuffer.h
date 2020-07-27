#pragma once

#include "Ares/Renderer/FrameBuffer.h"

namespace Ares
{
	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(const FrameBufferSpecs& specs, FramebufferFormat format);
		virtual ~OpenGLFrameBuffer();
		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void BindTexture(uint32_t slot = 0) const override;

		virtual bool Resize(uint32_t width, uint32_t height, bool force=false) override;
		
		virtual uint32_t GetRendererID() const { return m_RendererID; }
		virtual uint32_t GetColorAttachmentRendererID() const { return m_ColorAttachment; }
		virtual uint32_t GetDepthAttachmentRendererID() const { return m_DepthAttachment; }

		virtual uint32_t GetWidth() const { return m_Specs.Width; }
		virtual uint32_t GetHeight() const { return m_Specs.Height; }
		virtual FramebufferFormat GetFormat() const { return m_Format; }

		virtual const FrameBufferSpecs& GetSpecs() const override { return m_Specs; }
	private:
		uint32_t m_RendererID = 0;
		uint32_t m_ColorAttachment = 0, m_DepthAttachment = 0;
		FrameBufferSpecs m_Specs;
		FramebufferFormat m_Format;
	};
}