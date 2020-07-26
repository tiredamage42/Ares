
#include "AresPCH.h"
#include "Ares/Renderer/VertexArray.h"
#include "Ares/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
namespace Ares {

	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{

		case RendererAPI::API::None:
			ARES_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLVertexArray>();
		}

		ARES_CORE_ASSERT(false, "Unknow RendererAPI");
		return nullptr;
	}
}