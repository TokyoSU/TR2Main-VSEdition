#include "precompiled.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static std::shared_ptr<spdlog::logger> m_log = spdlog::basic_logger_mt("file", "logs/debug.txt", true);
static std::string m_debug_prevmsg, m_warn_prevmsg;

void LogInit()
{
#if defined(_DEBUG)
	m_log->set_level(spdlog::level::debug);
#else
	m_log->set_level(spdlog::level::warn);
#endif
}

void LogDebug(const char* message, ...)
{
#if defined(_DEBUG)
	char buffer[512];
	va_list args;
	va_start(args, message);
	vsprintf_s(buffer, message, args);
	va_end(args);
	if (m_debug_prevmsg == buffer) // avoid spamming... (From TEN)
		return;
	m_log->debug(buffer);
	m_debug_prevmsg = buffer;
#endif
}

void LogWarn(const char* message, ...)
{
	char buffer[512];
	va_list args;
	va_start(args, message);
	vsprintf_s(buffer, message, args);
	va_end(args);
	if (m_warn_prevmsg == buffer) // avoid spamming... (From TEN)
		return;
	m_log->warn(buffer);
	m_warn_prevmsg = buffer;
}
