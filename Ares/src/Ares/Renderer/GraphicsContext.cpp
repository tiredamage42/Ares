#include "AresPCH.h"
#include "Ares/Renderer/GraphicsContext.h"
#include "Ares/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Ares
{
	Scope<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    
			ARES_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); 
			return nullptr;
		case RendererAPI::API::OpenGL:  
			return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}

		ARES_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}