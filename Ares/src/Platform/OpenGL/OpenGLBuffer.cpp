#include "AresPCH.h"
#include "Platform/OpenGL/OpenGLBuffer.h"
#include <glad/glad.h>

#include "Ares/Renderer/Renderer.h"

namespace Ares {

	// =====================================================
	// VERTEX BUFFERS =======================================
	// =====================================================
	static GLenum OpenGLUsage(VertexBufferUsage usage)
	{
		switch (usage)
		{
		case VertexBufferUsage::Static:    return GL_STATIC_DRAW;
		case VertexBufferUsage::Dynamic:   return GL_DYNAMIC_DRAW;
		}
		ARES_CORE_ASSERT(false, "Unknown vertex buffer usage");
		return 0;
	}


	OpenGLVertexBuffer::OpenGLVertexBuffer(void* vertices, uint32_t size, VertexBufferUsage usage)
	{
		m_LocalData = Buffer::Copy(vertices, size);
		Renderer::Submit([this, size, vertices, usage]() mutable {
			glCreateBuffers(1, &this->m_RendererID);
			glNamedBufferData(this->m_RendererID, size, this->m_LocalData.Data, OpenGLUsage(usage));
		});
	}
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size, VertexBufferUsage usage)
	{
		Renderer::Submit([this, size, usage]() mutable {
			glCreateBuffers(1, &this->m_RendererID);
			glNamedBufferData(this->m_RendererID, size, nullptr, OpenGLUsage(usage));
		});
	}
	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		GLuint rendererID = m_RendererID;
		Renderer::Submit([rendererID]() {
			glDeleteBuffers(1, &rendererID);
		});
	}
	void OpenGLVertexBuffer::Bind() const
	{
		Renderer::Submit([this]() {
			glBindBuffer(GL_ARRAY_BUFFER, this->m_RendererID);
		});
	}
	void OpenGLVertexBuffer::Unbind() const
	{
		Renderer::Submit([]() {
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		});
	}

	void OpenGLVertexBuffer::SetData(void* data, uint32_t size, uint32_t offset)
	{
		m_LocalData = Buffer::Copy(data, size);
		Renderer::Submit([this, size, offset]() {
			glNamedBufferSubData(this->m_RendererID, offset, size, this->m_LocalData.Data);
		});
	}
	
	// =====================================================
	// INDEX BUFFERS =======================================
	// =====================================================

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indicies, uint32_t count)
		: m_Count(count)
	{
		m_LocalData = Buffer::Copy(indicies, count * sizeof(uint32_t));
		Renderer::Submit([this, count]() mutable {
			glCreateBuffers(1, &this->m_RendererID);
			glNamedBufferData(this->m_RendererID, count * sizeof(uint32_t), this->m_LocalData.Data, GL_STATIC_DRAW);
		});
	}
	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		GLuint rendererID = m_RendererID;
		Renderer::Submit([rendererID]() {
			glDeleteBuffers(1, &rendererID);
		});
	}
	void OpenGLIndexBuffer::Bind() const
	{
		Renderer::Submit([this]() {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_RendererID);
		});
	}
	void OpenGLIndexBuffer::Unbind() const
	{
		Renderer::Submit([]() {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		});
	}
}