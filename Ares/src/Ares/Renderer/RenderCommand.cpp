#include "AresPCH.h"
#include "Ares/Renderer/RenderCommand.h"

namespace Ares {
	Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
}