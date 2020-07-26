#include "AresPCH.h"
#include "Core.h"
#include "Log.h"

#define ARES_BUILD_ID "v0.1a"

namespace Ares
{
	void InitializeCore()
	{
		ARES_LOG_INIT();
		ARES_CORE_LOG("Hazel Engine {}", ARES_BUILD_ID);
		ARES_CORE_LOG("Initializing...");
	}

	void ShutdownCore()
	{
		ARES_CORE_LOG("Shutting down...");
	}
}