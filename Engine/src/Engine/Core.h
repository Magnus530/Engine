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