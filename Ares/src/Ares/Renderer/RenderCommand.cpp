#include "AresPCH.h"

#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Ares {
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI();
}