#pragma once

#ifdef E_PLATFORM_WINDOWS
	#ifdef E_BUILD_DLL
		#define ENGINE_API __declspec(dllexport)
	#else
		#define ENGINE_API __declspec(dllimport)
	#endif
#else
	#error The engine only supports Windows.
#endif

#ifdef E_ENABLE_ASSERTS
	#define E_ASSERT(x, ...) { if(!(x)) { E_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); }}
	#define E_CORE_ASSERT(x, ...) { if(!(x)) { E_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); }}
#else
	#define E_ASSERT(x, ...)
	#define E_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)