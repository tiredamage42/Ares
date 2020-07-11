#pragma once
#include "Core.h"

// suppress warnings
#pragma warning(push, 0)
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#pragma warning(pop)

namespace Ares {
	class ARES_API Log
	{
	public:
		static void Init();
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

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
