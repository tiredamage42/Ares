#include "AresPCH.h"

#include "Ares/Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Ares {
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

	Scope<RendererAPI> RendererAPI::Create()
	{
		switch (s_API)
		{
			case RendererAPI::API::None:    
				ARES_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); 
				return nullptr;
			case RendererAPI::API::OpenGL:  
				return CreateScope<OpenGLRendererAPI>();
		}

		ARES_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}