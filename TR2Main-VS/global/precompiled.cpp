#include "precompiled.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

static std::shared_ptr<spdlog::logger> m_log = spdlog::basic_logger_mt("file", "logs/debug.txt", true);

void Log(const char* message, ...)
{
	char buffer[512];
	va_list args;
	va_start(args, message);
	vsprintf_s(buffer, message, args);
	va_end(args);
	m_log->set_level(spdlog::level::trace);
	m_log->debug(buffer);
}