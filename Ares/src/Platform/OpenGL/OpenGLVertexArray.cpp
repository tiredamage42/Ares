#include "AresPCH.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"
#include <glad/glad.h>
#include "Ares/Renderer/Renderer.h"
namespace Ares {


	OpenGLVertexArray::OpenGLVertexArray()
	{
		Renderer::Submit([this]() mutable {
			glCreateVertexArrays(1, &this->m_RendererID);
		});
	}
	OpenGLVertexArray::~OpenGLVertexArray()
	{
		GLuint rendererID = m_RendererID;
		Renderer::Submit([rendererID]() {
			glDeleteVertexArrays(1, &rendererID);
		});
	}

	void OpenGLVertexArray::Bind() const
	{
		Renderer::Submit([this]() {
			glBindVertexArray(this->m_RendererID);
		});
	}
	void OpenGLVertexArray::Unbind() const
	{
		Renderer::Submit([]() {
			glBindVertexArray(0);
		});
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

		Renderer::Submit([this, buffer]() mutable {
			const auto& layout = buffer->GetLayout();

			for (const auto& element : layout)
			{
				switch (element.Type)
				{
				case ShaderDataType::Float:
				case ShaderDataType::Float2:
				case ShaderDataType::Float3:
				case ShaderDataType::Float4:
				case ShaderDataType::Int:
				case ShaderDataType::Int2:
				case ShaderDataType::Int3:
				case ShaderDataType::Int4:
				case ShaderDataType::Bool:
				{
					glEnableVertexAttribArray(this->m_VertexBufferIndex);
					glVertexAttribPointer(this->m_VertexBufferIndex,
						element.GetComponentCount(),
						ShaderDataType2OpenGLBaseType(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE,
						layout.GetStride(),
						(const void*)element.Offset);
					this->m_VertexBufferIndex++;
					break;
				}
				case ShaderDataType::Mat3:
				case ShaderDataType::Mat4:
				{
					uint8_t count = element.GetComponentCount();
					for (uint8_t i = 0; i < count; i++)
					{
						glEnableVertexAttribArray(this->m_VertexBufferIndex);
						glVertexAttribPointer(this->m_VertexBufferIndex,
							count,
							ShaderDataType2OpenGLBaseType(element.Type),
							element.Normalized ? GL_TRUE : GL_FALSE,
							layout.GetStride(),
							(const void*)(sizeof(float) * count * i));
						glVertexAttribDivisor(this->m_VertexBufferIndex, 1);
						this->m_VertexBufferIndex++;
					}
					break;
				}
				default:
					ARES_CORE_ASSERT(false, "Unknown ShaderDataType!");
				}
			}
		});
		m_VertexBuffers.push_back(buffer);
	}
	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& buffer)
	{
		Bind();
		buffer->Bind();
		m_IndexBuffer = buffer;
	}
}