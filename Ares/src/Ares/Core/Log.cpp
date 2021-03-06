#include "AresPCH.h"
#include "Ares/Core/Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Ares {
	Ref<spdlog::logger> Log::s_CoreLogger;
	Ref<spdlog::logger> Log::s_ClientLogger;


	/*template <typename Mutex>
	void Log::AddSink(std::shared_ptr<spdlog::sinks::base_sink<Mutex>> sink)
	{
		s_CoreLogger->sinks().push_pack(sink);
		s_ClientLogger->sinks().push_pack(sink);
	}*/

	void Log::Init() {

		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Ares.log", true));






		logSinks[0]->set_pattern("%^[%T] %n: %v%$"); // logs ::  [time stamp] log name: message
		logSinks[1]->set_pattern("[%T] [%l] %n: %v"); // files :: [time stamp] [log level] logname: message

		//spdlog::set_pattern("%^[%l][%T] %n: %v %g%#%!%$");

		s_CoreLogger = std::make_shared<spdlog::logger>("ARES", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_CoreLogger);
		//s_CoreLogger = spdlog::stdout_color_mt("ARES");
	
		s_CoreLogger->set_level(spdlog::level::trace);
		s_CoreLogger->flush_on(spdlog::level::trace);

		s_ClientLogger = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_ClientLogger);
		//s_ClientLogger = spdlog::stdout_color_mt("APP");

		s_ClientLogger->set_level(spdlog::level::trace);
		s_ClientLogger->flush_on(spdlog::level::trace);
	}
}