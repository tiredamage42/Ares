
#include "AresPCH.h"
#include "Ares/Renderer/FrameBuffer.h"

#include "Ares/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"
namespace Ares
{
	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{

		case RendererAPI::API::None:
			ARES_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLFrameBuffer>(spec);

		}

		ARES_CORE_ASSERT(false, "Unknow RendererAPI");
		return nullptr;
		
	}
}