#pragma once

#include "MCI/Common.h"
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace MCI
{

	class Log
	{
	public:

		static void Init(const std::string& loggerName = "MCI");

		static Ref<spdlog::logger> GetLogger();

	private:

		static Ref<spdlog::logger> s_logger;

	};

#pragma warning(pop)

#define MCI_LOG_DEBUG(...) MCI::Log::GetLogger()->debug(__VA_ARGS__)
#define MCI_LOG_TRACE(...) MCI::Log::GetLogger()->trace(__VA_ARGS__)
#define MCI_LOG_INFO(...)  MCI::Log::GetLogger()->info(__VA_ARGS__)
#define MCI_LOG_WARN(...)  MCI::Log::GetLogger()->warn(__VA_ARGS__)
#define MCI_LOG_ERROR(...) MCI::Log::GetLogger()->error(__VA_ARGS__)
#define MCI_LOG_FATAL(...) MCI::Log::GetLogger()->critical(__VA_ARGS__)

}