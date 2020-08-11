#pragma once

#include "Ares/Core/Core.h"

#ifdef ARES_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#include <string>

namespace Ares
{
	class FileSystemWatcher
	{
	public:
		void Watch();
	private:
		/* Runs On A Separate Thread To Ensure It Doesnt Block The Main Thread __WIN32 Only*/
		static DWORD WINAPI StartWatcherThread(LPVOID lparam);
		static std::string wchar_to_string(wchar_t* input);
	};
}
