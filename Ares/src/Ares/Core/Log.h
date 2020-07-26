#pragma once
#include "Ares/Core/Core.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Ares {
	class Log
	{
	public:
		static void Init();
		inline static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};
}

#define ARES_LOG_INIT() Ares::Log::Init();

// core engine loggers
#define ARES_CORE_LOG(...)		::Ares::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ARES_CORE_INFO(...)		::Ares::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ARES_CORE_WARN(...)		::Ares::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ARES_CORE_ERROR(...)	::Ares::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ARES_CORE_CRITICAL(...)	::Ares::Log::GetCoreLogger()->critical(__VA_ARGS__)

// client loggers
#define ARES_LOG(...)		::Ares::Log::GetClientLogger()->trace(__VA_ARGS__)
#define ARES_INFO(...)		::Ares::Log::GetClientLogger()->info(__VA_ARGS__)
#define ARES_WARN(...)		::Ares::Log::GetClientLogger()->warn(__VA_ARGS__)
#define ARES_ERROR(...)		::Ares::Log::GetClientLogger()->error(__VA_ARGS__)
#define ARES_CRITICAL(...)	::Ares::Log::GetClientLogger()->critical(__VA_ARGS__)
