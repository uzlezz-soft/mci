#pragma once

#include "MCI/Common.h"
#include "MCI/Log.h"
#include <filesystem>

#define MCI_EXPAND_MACRO(x) x
#define MCI_STRINGIFY_MACRO(x) #x

#define GET_LOG_MACRO_ERROR(x) MCI_LOG##x##FATAL

#ifdef MCI_ENABLE_ASSERTS
#define MCI_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { MCI_EXPAND_MACRO(MCI_EXPAND_MACRO(GET_LOG_MACRO_ERROR(type))(msg, __VA_ARGS__)); MCI_DEBUGBREAK(); } }
#define MCI_INTERNAL_ASSERT_WITH_MSG(type, check, ...) MCI_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define MCI_INTERNAL_ASSERT_NO_MSG(type, check, ...) MCI_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", MCI_STRINGIFY_MACRO(check), std::string(PROJECT_NAME).append("/").append(std::filesystem::path(__FILE__).filename().generic_string()), __LINE__)

#define MCI_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define MCI_INTERNAL_ASSERT_GET_MACRO(...) MCI_EXPAND_MACRO( MCI_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, MCI_INTERNAL_ASSERT_WITH_MSG, MCI_INTERNAL_ASSERT_NO_MSG) )

#define MCI_ASSERT(expression, ...) MCI_EXPAND_MACRO( MCI_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, expression, __VA_ARGS__) )

#define MCI_PURE_VIRTUAL(...) { GET_LOG_MACRO_ERROR(_)("{0}: The method is pure virtual and should be overriden in child classes!", GetType()->GetName()); MCI_DEBUGBREAK(); __VA_ARGS__; }

#else
#define MCI_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { MCI_EXPAND_MACRO(MCI_EXPAND_MACRO(GET_LOG_MACRO_ERROR(type))(msg, __VA_ARGS__)); } }
#define MCI_INTERNAL_ASSERT_WITH_MSG(type, check, ...) MCI_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define MCI_INTERNAL_ASSERT_NO_MSG(type, check, ...) MCI_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", MCI_STRINGIFY_MACRO(check), std::string(PROJECT_NAME).append("/").append(std::filesystem::path(__FILE__).filename().generic_string()), __LINE__)

#define MCI_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define MCI_INTERNAL_ASSERT_GET_MACRO(...) MCI_EXPAND_MACRO( MCI_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, MCI_INTERNAL_ASSERT_WITH_MSG, MCI_INTERNAL_ASSERT_NO_MSG) )

#define MCI_ASSERT(expression, ...) MCI_EXPAND_MACRO( MCI_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, expression, __VA_ARGS__) )

#define MCI_PURE_VIRTUAL() { MCI_BREAKPOINT(); }
#endif