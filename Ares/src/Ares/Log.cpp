#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
namespace Ares {
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init() {
		spdlog::set_pattern("%^[%l][%T] %n: %v %g%#%!%$");
	
		s_CoreLogger = spdlog::stdout_color_mt("ARES");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("APP");
		s_CoreLogger->set_level(spdlog::level::trace);
	}
}