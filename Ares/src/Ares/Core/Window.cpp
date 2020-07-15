#include "AresPCH.h"
#include "Ares/Core/Window.h"

#ifdef ARES_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Ares
{

	Scope<Window> Window::Create(const WindowProps& props)
	{
#ifdef ARES_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(props);
#else
		ARES_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}

}