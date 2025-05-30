#pragma once

#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

class Logger
{
public:
    static void Init()
    {
        if (!s_Logger)
        {
            s_Logger = spdlog::stdout_color_mt("Core");
            s_Logger->set_level(spdlog::level::trace); // Log everything
            s_Logger->set_pattern("[%T] [%l] %^%v%$");
        }
    }

    inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }

private:
    static std::shared_ptr<spdlog::logger> s_Logger;
};

inline std::shared_ptr<spdlog::logger> Logger::s_Logger = nullptr;

#ifndef CONFIGURATION_DIST
#define LOG_TRACE(...)    Logger::GetLogger()->trace(__VA_ARGS__)
#define LOG_DEBUG(...)    Logger::GetLogger()->debug(__VA_ARGS__)
#define LOG_INFO(...)     Logger::GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)     Logger::GetLogger()->warn(__VA_ARGS__)
#endif
#define LOG_ERROR(...)    Logger::GetLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) Logger::GetLogger()->critical(__VA_ARGS__)
