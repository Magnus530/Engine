#pragma once


#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Engine
{
	class ENGINE_API Log
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

// Core log macros.
#define E_CORE_TRACE(...)	::Engine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define E_CORE_INFO(...)	::Engine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define E_CORE_WARN(...)	::Engine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define E_CORE_ERROR(...)	::Engine::Log::GetCoreLogger()->error(__VA_ARGS__)
//#define E_CORE_FATAL(...)	::Engine::Log::GetCoreLogger()->fatal(__VA_ARGS__) // fatal finnes ikke

// Client log macros.
#define E_TRACE(...)		::Engine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define E_INFO(...)			::Engine::Log::GetClientLogger()->info(__VA_ARGS__)
#define E_WARN(...)			::Engine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define E_ERROR(...)		::Engine::Log::GetClientLogger()->error(__VA_ARGS__)
//#define E_FATAL(...)		::Engine::Log::GetClientLogger()->fatal(__VA_ARGS__) // fatal finnes ikke
