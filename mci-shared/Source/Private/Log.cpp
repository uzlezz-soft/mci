#include "MCI/Log.h"
#pragma warning(push, 0)
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#pragma warning(pop)

namespace MCI
{

	Ref<spdlog::logger> Log::s_logger;

	void Log::Init(const std::string& loggerName)
	{
		s_logger = spdlog::stdout_color_mt(loggerName);
		s_logger->set_level(spdlog::level::trace);
		s_logger->set_pattern("%^[%T] %n: %v%$");
	}

	Ref<spdlog::logger> Log::GetLogger()
	{
		return s_logger;
	}

}