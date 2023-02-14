#pragma once

#include "MCI/Common.h"
#include "MCI/Assert.h"
#pragma warning(push, 0)
#include <glad/glad.h>
#pragma warning(pop)

namespace MCI
{

#define OPENGL_VERSION_MAJOR 4
#define OPENGL_VERSION_MINOR 6
#define OPENGL_VERSION_SHADER_STRING "#version 460 core"

#ifndef MCI_SHIPPING

	const char* OpenGLErrorToString(uint32 err) noexcept;

#define GLCall(x) do { while (glGetError() != 0); x; auto __error = glGetError(); if (__error != 0) MCI_LOG_ERROR("OpenGLDebug: File {} Line {}: {}", std::filesystem::path(__FILE__).filename(), __LINE__, OpenGLErrorToString(__error)); } while (0)
#else
#define GLCall(x) x
#endif

}