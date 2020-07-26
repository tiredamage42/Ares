#include "AresPCH.h"
#include "Platform/OpenGL/OpenGLBuffer.h"
#include <glad/glad.h>

#include "Ares/Renderer/Renderer.h"

namespace Ares {

	// =====================================================
	// VERTEX BUFFERS =======================================
	// =====================================================


	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
	{
		Renderer::Submit([this, size, vertices]() mutable {

			glCreateBuffers(1, &this->m_RendererID);
			glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
			// upload to gpu
			glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
		});
	}
	// dynamic buffer
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		Renderer::Submit([this, size]() mutable {

		glCreateBuffers(1, &this->m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, this->m_RendererID);
		// upload to gpu
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
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

	void OpenGLVertexBuffer::SetData(void* data, uint32_t size)
	{

		m_LocalData = Buffer::Copy(data, size);

		Renderer::Submit([this, size]() {

		glBindBuffer(GL_ARRAY_BUFFER, this->m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, this->m_LocalData.Data);
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

		// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
		// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state

		glCreateBuffers(1, &this->m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, this->m_RendererID);

		// upload to gpu
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), this->m_LocalData.Data, GL_STATIC_DRAW);
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