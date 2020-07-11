
#include "AresPCH.h"
#include "VertexArray.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
namespace Ares {

	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{

		case RendererAPI::None:
			ARES_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

		case RendererAPI::OpenGL:
			return new OpenGLVertexArray();

		}

		ARES_CORE_ASSERT(false, "Unknow RendererAPI");
		return nullptr;
	}
}