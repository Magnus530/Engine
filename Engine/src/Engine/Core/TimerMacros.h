#pragma once
/* -- timer macros --
*	TIMER t1 = TIMENOW()
*	TIME_MILLI("text", t1) */

typedef std::chrono::high_resolution_clock::time_point TIMER;
#define DURATION(a) std::chrono::duration_cast<std::chrono::nanoseconds>(a).count()
#define TIMENOW() std::chrono::high_resolution_clock::now()
#define TIME_NANO(x, t)		E_TRACE(std::string(x) + "{0} nanoseconds",				DURATION(TIMENOW() - t))				
#define TIME_MICRO(x, t)	E_TRACE(std::string(x) + "{0} microseconds",	(float)(DURATION(TIMENOW() - t) / (1e3)))		
#define TIME_MILLI(x, t)	E_TRACE(std::string(x) + "{0} milliseconds",	(float)(DURATION(TIMENOW() - t) / (1e6)))		
#define TIME_SECOND(x, t)	E_TRACE(std::string(x) + "{0} seconds",			(float)(DURATION(TIMENOW() - t) / (1e9)))	
