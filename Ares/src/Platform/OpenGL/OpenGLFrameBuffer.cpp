#include "AresPCH.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"

//#include "Ares/Renderer/Renderer.h"
#include <glad/glad.h>

namespace Ares
{
	static const uint32_t s_MaxFrameBufferSize = 8192;
	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecs& specs)
		: m_Specs(specs)
	{
		//Invalidate();
		Resize(specs.Width, specs.Height);
	}
	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{

		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(1, &m_ColorAttachment);
		glDeleteTextures(1, &m_DepthAttachment);
		
	}

	void OpenGLFrameBuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specs.Width, m_Specs.Height);
	}
	void OpenGLFrameBuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFrameBufferSize || height > s_MaxFrameBufferSize)
		{
			ARES_CORE_WARN("Attempted to resize frame buffer to w: {0}, h: {1}", width, height);
			return;
		}
		if (m_Specs.Width == width && m_Specs.Height == height)
			return;

		m_Specs.Width = width;
		m_Specs.Height = height;


		//Invalidate();

		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(1, &m_ColorAttachment);
			glDeleteTextures(1, &m_DepthAttachment);
		}

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		// create the color attachment
		glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specs.Width, m_Specs.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// attach it
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

		// create the depth attachment
		glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Specs.Width, m_Specs.Height);

		// attach it
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

		ARES_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

		// unbind frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
	}
}