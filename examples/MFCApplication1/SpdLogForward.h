#pragma once


/*
2020/04/08 官网的issues看起来一直是忽视状态
%@,%s,%x 都不能用0.0
只能自己定义了
*/
#define SPDLOG_STR_H(x) #x
#define SPDLOG_STR_HELPER(x) SPDLOG_STR_H(x)

#define LOGGER_TRACE(logger, ...) logger->trace(__VA_ARGS__)
#define LOGGER_DEBUG(logger, ...) logger->debug(__VA_ARGS__)
#define LOGGER_INFO(logger, ...) logger->info(__VA_ARGS__ )
#define LOGGER_WARN(logger, ...) logger->warn(__VA_ARGS__)
#define LOGGER_ERROR(logger, ...) logger->error(__VA_ARGS__)
#define LOGGER_FATAL(logger, ...) logger->critical(__VA_ARGS__)

#define LOG_TRACE(...) spdlog::trace(__VA_ARGS__ ,"[ " __FILE__ "(" SPDLOG_STR_HELPER(__LINE__) ") ] " )
#define LOG_DEBUG(...) spdlog::debug(__VA_ARGS__"[ " __FILE__ "(" SPDLOG_STR_HELPER(__LINE__) ") ] " )
#define LOG_INFO(...) spdlog::info(__VA_ARGS__"[ " __FILE__ "(" SPDLOG_STR_HELPER(__LINE__) ") ] ")
#define LOG_WARN(...) spdlog::warn(__VA_ARGS__"[ " __FILE__ "(" SPDLOG_STR_HELPER(__LINE__) ") ] ")
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__"[ " __FILE__ "(" SPDLOG_STR_HELPER(__LINE__) ") ] " )
#define LOG_FATAL(...) spdlog::critical(__VA_ARGS__"[ " __FILE__ "(" SPDLOG_STR_HELPER(__LINE__) ") ] " )

//uncomment the line follow, if you want see something weird
//#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"