#pragma once

#ifdef E_PLATFORM_WINDOWS
#if E_DYNAMIC_LINK
	#ifdef E_BUILD_DLL
		#define ENGINE_API __declspec(dllexport)
	#else
		#define ENGINE_API __declspec(dllimport)
	#endif
#else
	#define ENGINE_API
#endif
#else
	#error The engine only supports Windows.
#endif

#ifdef E_DEBUG
	#define E_ENABLE_ASSERTS
#endif

#ifdef E_ENABLE_ASSERTS
	#define E_ASSERT(x, ...) { if(!(x)) { E_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); }}
	#define E_CORE_ASSERT(x, ...) { if(!(x)) { E_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); }}
#else
	#define E_ASSERT(x, ...)
	#define E_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define E_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)