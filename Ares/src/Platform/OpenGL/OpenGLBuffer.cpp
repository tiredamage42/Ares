#include "AresPCH.h"
#include "OpenGLBuffer.h"
#include <glad/glad.h>

namespace Ares {

	// =====================================================
	// VERTEX BUFFERS =======================================
	// =====================================================

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);

		// upload to gpu
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}
	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}
	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}
	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	
	// =====================================================
	// INDEX BUFFERS =======================================
	// =====================================================

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indicies, uint32_t count)
		: m_Count(count)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);

		// upload to gpu
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indicies, GL_STATIC_DRAW);
	}
	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}
	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}
	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}