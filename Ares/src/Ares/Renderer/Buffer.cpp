
#include "AresPCH.h"
#include "Ares/Renderer/Buffer.h"
#include "Ares/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Ares 
{
	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{

		case RendererAPI::API::None:
			ARES_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLVertexBuffer>(size);

		}

		ARES_CORE_ASSERT(false, "Unknow RendererAPI");
		return nullptr;
	}
	Ref<VertexBuffer> VertexBuffer::Create(void* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{

		case RendererAPI::API::None:
			ARES_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLVertexBuffer>(vertices, size);

		}

		ARES_CORE_ASSERT(false, "Unknow RendererAPI");
		return nullptr;
	}
	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indicies, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{

		case RendererAPI::API::None:
			ARES_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLIndexBuffer>(indicies, count);
		}

		ARES_CORE_ASSERT(false, "Unknow RendererAPI");
		return nullptr;

	}
}