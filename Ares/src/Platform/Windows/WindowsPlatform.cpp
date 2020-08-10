#include "AresPCH.h"
#include "Ares/Core/Platform.h"
#include <GLFW/glfw3.h>

namespace Ares
{
	double Platform::GetTime()
	{
		return glfwGetTime();
	}
	const char* Platform::GetName()
	{
		return "Windows x64";
	}
}