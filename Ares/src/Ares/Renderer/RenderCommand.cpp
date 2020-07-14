#include "AresPCH.h"

#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Ares {
	Scope<RendererAPI> RenderCommand::s_RendererAPI = CreateScope<OpenGLRendererAPI>();
}