#pragma once

#include "MCI/PlatformDetection.h"
#include "MCI/Timer.h"
#include <memory>
#include <cstdint>
#include <filesystem>

#if defined(MCI_PLATFORM_WINDOWS)
#define MCI_DEBUGBREAK() __debugbreak()
#elif defined(MCIPLATFORM_LINUX)
#include <signal.h>
#define MCI_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif

#define MCI_ALIGN(x) alignas(x)

#define MCI_EXPAND_MACRO(x) x
#define MCI_STRINGIFY_MACRO(x) #x
#define MCI_CONCAT_MACRO(a, b) a ## b

namespace MCI
{

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> MakeRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> MakeScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	using int8 = int8_t;
	using int16 = int16_t;
	using int32 = int32_t;
	using int64 = int64_t;
	using uint8 = uint8_t;
	using uint16 = uint16_t;
	using uint32 = uint32_t;
	using uint64 = uint64_t;

}

#define MCI_BIT(x) (1 << x)

#define NON_COPYABLE(T) T(const T&) = delete; T& operator=(const T&) = delete;
#define CALL_ONLY_ONCE() do { static bool __ = false; if (__) return; __ = true; } while(0)

#if defined(MCI_DEVELOPMENT)
#define MCI_ENABLE_ASSERTS
#endif