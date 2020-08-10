#pragma once


#include "Ares/Renderer/Buffer.h"
#include "Ares/Core/Buffer.h"

namespace Ares {

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(void* vertices, uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Static);
		OpenGLVertexBuffer(uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Dynamic);

		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

		virtual void SetData(void* data, uint32_t size, uint32_t offset = 0) override;
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

	private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;

		Buffer m_LocalData;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:

		OpenGLIndexBuffer(uint32_t count);
		OpenGLIndexBuffer(uint32_t* indicies, uint32_t count);

		virtual ~OpenGLIndexBuffer();

		virtual void SetData(void* data, uint32_t count, uint32_t offset = 0);
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual uint32_t GetCount() const { return m_Count; }

	private:
		uint32_t m_RendererID;
		uint32_t m_Count;

		Buffer m_LocalData;
	};

}