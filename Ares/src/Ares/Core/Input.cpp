
//#include "AresPCH.h"
//#include "Ares/Core/Input.h"
//
//#ifdef ARES_PLATFORM_WINDOWS
//	#include "Platform/Windows/WindowsInput.h"
//#endif
//namespace Ares
//{
//	Scope<Input> Input::s_Instance = Input::Create();
//
//	Scope<Input> Input::Create()
//	{
//#ifdef ARES_PLATFORM_WINDOWS
//		return CreateScope<WindowsInput>();
//#else
//		ARES_CORE_ASSERT(false, "Unknown platform!");
//		return nullptr;
//#endif
//	}
//}