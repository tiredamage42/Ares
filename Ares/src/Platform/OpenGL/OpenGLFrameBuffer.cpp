#include "AresPCH.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"

#include "Ares/Renderer/Renderer.h"
#include <glad/glad.h>

namespace Ares
{
	static const uint32_t s_MaxFrameBufferSize = 8192;

	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecs& specs)
		: m_Specs(specs)
	{
		Resize(specs.Width, specs.Height, true);
	}
	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		uint32_t rendererID = m_RendererID;
		uint32_t colorAttachment = m_ColorAttachment;
		uint32_t depthAttachment = m_DepthAttachment;

		Renderer::Submit([=]() {
			glDeleteFramebuffers(1, &rendererID);
			glDeleteTextures(1, &colorAttachment);
			glDeleteTextures(1, &depthAttachment);
		});
	}

	void OpenGLFrameBuffer::Bind() const
	{
		Renderer::Submit([this]() {
			glBindFramebuffer(GL_FRAMEBUFFER, this->m_RendererID);
			glViewport(0, 0, this->m_Specs.Width, this->m_Specs.Height);
		});
	}
	void OpenGLFrameBuffer::Unbind() const
	{
		Renderer::Submit([]() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		});
	}

	void OpenGLFrameBuffer::BindAsTexture(uint32_t slot) const
	{
		Renderer::Submit([this, slot]() {
			glBindTextureUnit(slot, m_ColorAttachment);
			/*glActiveTexture(GL_TEXTURE0 + slot);
			glBindTexture(GL_TEXTURE_2D, this->m_ColorAttachment);*/
		});
	}

	bool OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height, bool force)
	{
		if (width == 0 || height == 0 || width > s_MaxFrameBufferSize || height > s_MaxFrameBufferSize)
		{
			ARES_CORE_WARN("Attempted to resize frame buffer to w: {0}, h: {1}", width, height);
			return false;
		}
		if (m_Specs.Width == width && m_Specs.Height == height && !force)
			return false;

		m_Specs.Width = width;
		m_Specs.Height = height;

		Renderer::Submit([this]() {

			if (this->m_RendererID)
			{
				glDeleteFramebuffers(1, &this->m_RendererID);
				glDeleteTextures(1, &this->m_ColorAttachment);
				glDeleteTextures(1, &this->m_DepthAttachment);
			}

			glCreateFramebuffers(1, &this->m_RendererID);
			glBindFramebuffer(GL_FRAMEBUFFER, this->m_RendererID);


			bool multisample = this->m_Specs.Samples > 1;
			if (multisample)
			{
				glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_ColorAttachment);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachment);

				// TODO: Create texture object based on format here
				if (m_Specs.Format == FramebufferFormat::RGBA16F)
				{
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Specs.Samples, GL_RGBA16F, m_Specs.Width, m_Specs.Height, GL_FALSE);
					//glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Specification.Samples, GL_RGBA16F, m_Specification.Width, m_Specification.Height, GL_FALSE);
				}
				else if (m_Specs.Format == FramebufferFormat::RGBA8)
				{
					// glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGBA8, m_Specification.Width, m_Specification.Height, GL_TRUE);
					glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Specs.Samples, GL_RGBA8, m_Specs.Width, m_Specs.Height, GL_FALSE);
				}
				// glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				// glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachment, 0);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
			}
			else
			{

				// create the color attachment
				glCreateTextures(GL_TEXTURE_2D, 1, &this->m_ColorAttachment);
				glBindTexture(GL_TEXTURE_2D, this->m_ColorAttachment);

				// TODO: Create texture object based on format here
				if (this->GetFormat() == FramebufferFormat::RGBA16F)
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, this->m_Specs.Width, this->m_Specs.Height, 0, GL_RGBA, GL_FLOAT, nullptr);
				}
				else if (this->GetFormat() == FramebufferFormat::RGBA8)
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->m_Specs.Width, this->m_Specs.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
				}
				//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, this->m_Specs.Width, this->m_Specs.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				// attach it
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->m_ColorAttachment, 0);
			}
			if (multisample)
			{
				glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_DepthAttachment);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_DepthAttachment);
				// glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height, GL_TRUE);
				glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Specs.Samples, GL_DEPTH24_STENCIL8, m_Specs.Width, m_Specs.Height, GL_FALSE);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
				// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, m_DepthAttachment, 0);
			}
			else
			{
				// create the depth attachment
				glCreateTextures(GL_TEXTURE_2D, 1, &this->m_DepthAttachment);
				glBindTexture(GL_TEXTURE_2D, this->m_DepthAttachment);


				//glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, this->m_Specs.Width, this->m_Specs.Height);
				//ALT
				glTexImage2D(
					GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, this->m_Specs.Width, this->m_Specs.Height, 0,
					GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
				);

				// attach it
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, this->m_DepthAttachment, 0);
			}

			if (multisample)
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachment, 0);
			else
				glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_ColorAttachment, 0);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, m_DepthAttachment, 0);



			ARES_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

			// unbind frame buffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		});

		return true;
		
	}
}