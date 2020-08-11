#pragma once

#include <string>

namespace Ares
{
	class EditorConsole {
	public:
		static void Log(std::string logLevel, std::string logMessage);
	};
}
