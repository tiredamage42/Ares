
#include "AresPCH.h"
#include "Ares/Renderer/FrameBuffer.h"

#include "Ares/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"
namespace Ares
{
	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecs& specs, FramebufferFormat format)
	{
		switch (Renderer::GetAPI())
		{

		case RendererAPI::API::None:
			ARES_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			Ref<FrameBuffer> result = CreateRef<OpenGLFrameBuffer>(specs, format);
			FramebufferPool::GetGlobal()->Add(result);
			return result;

		}

		ARES_CORE_ASSERT(false, "Unknow RendererAPI");
		return nullptr;
		
	}

	FramebufferPool* FramebufferPool::s_Instance = new FramebufferPool;

	FramebufferPool::FramebufferPool(uint32_t maxFBs /* = 32 */)
	{

	}

	FramebufferPool::~FramebufferPool()
	{

	}

	//std::weak_ptr<FrameBuffer> FramebufferPool::AllocateBuffer()
	//{
	//	// m_Pool.push_back();
	//	return std::weak_ptr<FrameBuffer>();
	//}

	void FramebufferPool::Add(Ref<FrameBuffer> framebuffer)
	{
		m_Pool.push_back(framebuffer);
	}
}