

#include "AresPCH.h"

#include "OpenGLVertexArray.h"
#include <glad/glad.h>

namespace Ares {


	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}
	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}
	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}




	static GLenum ShaderDataType2OpenGLBaseType(ShaderDataType type) {
		switch (type)
		{
		case Ares::ShaderDataType::Float:	return GL_FLOAT;
		case Ares::ShaderDataType::Float2:	return GL_FLOAT;
		case Ares::ShaderDataType::Float3:	return GL_FLOAT;
		case Ares::ShaderDataType::Float4:	return GL_FLOAT;
		case Ares::ShaderDataType::Mat3:	return GL_FLOAT;
		case Ares::ShaderDataType::Mat4:	return GL_FLOAT;
		case Ares::ShaderDataType::Int:		return GL_INT;
		case Ares::ShaderDataType::Int2:	return GL_INT;
		case Ares::ShaderDataType::Int3:	return GL_INT;
		case Ares::ShaderDataType::Int4:	return GL_INT;
		case Ares::ShaderDataType::Bool:	return GL_BOOL;
		}
		ARES_CORE_ASSERT(false, "Unknown ShaderDataType");
		return 0;
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& buffer)
	{
		ARES_CORE_ASSERT(buffer->GetLayout().GetElements().size(), "Vertex Buffer Has No Layout!");
		
		glBindVertexArray(m_RendererID);
		buffer->Bind();


		const auto& layout = buffer->GetLayout();

		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(m_VertexBufferIndex);
			glVertexAttribPointer(
				m_VertexBufferIndex,
				element.GetComponentCount(),
				ShaderDataType2OpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset
			);
			m_VertexBufferIndex++;
		}
		m_VertexBuffers.push_back(buffer);
	}
	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& buffer)
	{
		glBindVertexArray(m_RendererID);
		buffer->Bind();
		m_IndexBuffer = buffer;
	}
}