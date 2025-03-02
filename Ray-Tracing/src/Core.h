#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#ifdef WIN32
	#define PLATFORM_STRING "Windows"
	#define PLATFORM Windows
#else
	#define PLATFORM_STRING "Unknown Operating System"
	#define PLATFORM Unknown
	#error Other Operating Systems or Architectures are not supported yet
#endif


void Empty() { }

template<typename First, typename ... Object>
void Empty(First arg, const Object&... rest) { 
	Empty(rest...);
}

#ifdef DEBUG
	#undef IMGUI_DISABLE
#elif RELEASE
	#define IMGUI_DISABLE
#endif


// LOGGER

#ifdef DEBUG
	#define LOG_INFO spdlog::get("console")->info
	#define LOG_DEBUG spdlog::get("console")->debug
	#define LOG_WARN spdlog::get("console")->warn
	#define LOG_ERROR spdlog::get("console")->error
#elif RELEASE
	#define LOG_INFO Empty
	#define LOG_DEBUG Empty
	#define LOG_WARN Empty
	#define LOG_ERROR spdlog::get("console")->error
#endif

static void initLogger() {
	auto console = spdlog::stdout_color_mt("console");
	spdlog::set_pattern("[%H:%M:%S] [thread %t] %^%v%$");
	spdlog::set_level(spdlog::level::debug);
	LOG_INFO("Logger initialized!");
}