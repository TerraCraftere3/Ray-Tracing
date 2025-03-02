#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "Utils.h"

#if defined(WIN32)
	#define PLATFORM Windows
	#define PLATFORM_STRING "Windows"
#else
	#define PLATFORM Unknown
	#define PLATFORM_STRING "Unknown Operating System"
	#error Other Operating Systems or Architectures are not supported yet
#endif

#ifdef _DEBUG
	#define CONFIG Debug
	#define CONFIG_STRING "Debug"
	#define DEBUG
#elif NDEBUG
	#define CONFIG Release
	#define CONFIG_STRING "Release"
	#define RELEASE
#endif

#ifdef DEBUG
	#undef IMGUI_DISABLE
#elif defined(RELEASE)
	#define IMGUI_DISABLE
#endif


// LOGGER

//#ifdef DEBUG
	#define LOG_INFO spdlog::get("console")->info
	#define LOG_DEBUG spdlog::get("console")->debug
	#define LOG_WARN spdlog::get("console")->warn
	#define LOG_ERROR spdlog::get("console")->error
/*#elif defined(RELEASE)
	#define LOG_INFO Utils::Empty
	#define LOG_DEBUG Utils::Empty
	#define LOG_WARN Utils::Empty
	#define LOG_ERROR spdlog::get("console")->error
#endif*/

static void initLogger() {
	auto console = spdlog::stdout_color_mt("console");
	spdlog::set_pattern("[%H:%M:%S] [thread %t] %^%v%$");
	spdlog::set_level(spdlog::level::debug);
	LOG_INFO("Logger initialized!");
}