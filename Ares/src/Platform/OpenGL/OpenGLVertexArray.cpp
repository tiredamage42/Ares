#include "AresPCH.h"
#include "OpenGLVertexArray.h"

#include "Ares/Renderer/Renderer.h"

#include <glad/glad.h>

namespace Ares {

	OpenGLVertexArray::OpenGLVertexArray()
	{
		Renderer::Submit([=]() { glCreateVertexArrays(1, &m_RendererID); }, "Create Vertex ara");
	}
	OpenGLVertexArray::~OpenGLVertexArray()
	{
		GLuint rendererID = m_RendererID;
		Renderer::Submit([rendererID]() { glDeleteVertexArrays(1, &rendererID); }, "Delte vertex array");
	}

	void OpenGLVertexArray::Bind() const
	{
		Renderer::Submit([=]() { glBindVertexArray(m_RendererID); }, "Bind Vertex Array");
	}
	void OpenGLVertexArray::Unbind() const
	{
		Renderer::Submit([]() { glBindVertexArray(0); }, "Unbind Vertex Array");
	}

	static GLenum ShaderDataType2OpenGLBaseType(ShaderDataType type) {
		switch (type)
		{
			case ShaderDataType::Float:		return GL_FLOAT;
			case ShaderDataType::Float2:	return GL_FLOAT;
			case ShaderDataType::Float3:	return GL_FLOAT;
			case ShaderDataType::Float4:	return GL_FLOAT;
			case ShaderDataType::Mat3:		return GL_FLOAT;
			case ShaderDataType::Mat4:		return GL_FLOAT;
			case ShaderDataType::Int:		return GL_INT;
			case ShaderDataType::Int2:		return GL_INT;
			case ShaderDataType::Int3:		return GL_INT;
			case ShaderDataType::Int4:		return GL_INT;
			case ShaderDataType::Bool:		return GL_BOOL;
		}
		ARES_CORE_ASSERT(false, "Unknown ShaderDataType");
		return 0;
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& buffer)
	{
		ARES_CORE_ASSERT(buffer->GetLayout().GetElements().size(), "Vertex Buffer Has No Layout!");
		Bind();
		buffer->Bind();
		Renderer::Submit([=]() {

			const auto& layout = buffer->GetLayout();

			for (const auto& el : layout)
			{
				auto glBaseType = ShaderDataType2OpenGLBaseType(el.Type);
				glEnableVertexAttribArray(m_VertexBufferIndex);
				if (glBaseType == GL_INT)
				{
					glVertexAttribIPointer(m_VertexBufferIndex, el.GetComponentCount(), glBaseType, layout.GetStride(), (const void*)(intptr_t)el.Offset);
				}
				else
				{
					glVertexAttribPointer(m_VertexBufferIndex, el.GetComponentCount(), glBaseType, el.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)(intptr_t)el.Offset);
				}
				m_VertexBufferIndex++;
			}
		}, "Add Vertex Buffer To Vetex Array");

		m_VertexBuffers.push_back(buffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& buffer)
	{
		Bind();
		buffer->Bind();
		m_IndexBuffer = buffer;
	}
}