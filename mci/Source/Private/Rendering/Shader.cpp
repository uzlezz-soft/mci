#include "MCI/Rendering/Shader.h"
#include "OpenGL.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>

using namespace MCI;

static uint32_t MCIShaderTypeToOpenGLShaderType(Shader::Type type)
{
	switch (type)
	{
	case Shader::Type::Vertex: return GL_VERTEX_SHADER;
	case Shader::Type::Fragment: return GL_FRAGMENT_SHADER;
	case Shader::Type::Geometry: return GL_GEOMETRY_SHADER;
	}

	return 0;
}

static std::string_view MCIShaderTypeToString(Shader::Type type)
{
	switch (type)
	{
	case Shader::Type::Vertex: return "Vertex";
	case Shader::Type::Fragment: return "Fragment";
	case Shader::Type::Geometry: return "Geometry";
	}

	return 0;
}

Shader::Shader(const std::unordered_map<Type, std::string_view>& sources, const std::unordered_map<std::string_view, std::string>& parameters) : m_id(0)
{
	m_id = glCreateProgram();

	static std::unordered_set<Shader::Type> compiledShaders;
	static std::vector<uint32_t> compiledShaderIds;
	compiledShaders.clear();
	compiledShaderIds.clear();

	int32_t success = 0;

	for (auto& shaderSource : sources)
	{
		Shader::Type type = shaderSource.first;

		if (type == Shader::Type::Geometry)
		{
			MCI_ASSERT(compiledShaders.find(Shader::Type::Fragment) == compiledShaders.end(), 
				"Shader program already has fragment shader attached! Geometry shader should go first!");
		}
		
		std::stringstream ss;
		ss << OPENGL_VERSION_SHADER_STRING "\n";
		for (auto& parameter : parameters)
		{
			ss << "#define " << parameter.first << " " << parameter.second << "\n";
		}
		ss << shaderSource.second;
		std::string newSource = ss.str();

		const char* cSource = newSource.c_str();
		int32_t length = newSource.length();

		const int32_t infoLogLength = 1024;
		char infoLog[infoLogLength];

		uint32_t shader = glCreateShader(MCIShaderTypeToOpenGLShaderType(type));
		glShaderSource(shader, 1, &cSource, &length);
		glCompileShader(shader);

		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLog);
			MCI_LOG_ERROR("Shader of type {}: compilation failed. Log: {}\n", MCIShaderTypeToString(type), infoLog);
			MCI_DEBUGBREAK();

			return;
		};

		GLCall(glAttachShader(m_id, shader));

		compiledShaders.insert(type);
		compiledShaderIds.push_back(shader);
	}

	GLCall(glLinkProgram(m_id));
	glGetProgramiv(m_id, GL_LINK_STATUS, &success);
	if (!success)
	{
		const int32_t infoLogLength = 1024;
		char infoLog[infoLogLength];
		glGetProgramInfoLog(m_id, infoLogLength, NULL, infoLog);

		for (auto& id : compiledShaderIds)
		{
			glDetachShader(m_id, id);
			glDeleteShader(id);
		}

		MCI_LOG_ERROR("Cannot link shader! Log: {}\n", infoLog);
		MCI_DEBUGBREAK();

		return;
	}

	for (auto& id : compiledShaderIds)
	{
		glDeleteShader(id);
	}

	CacheUniforms();
}

Shader::~Shader()
{
	if (m_id != 0)
	{
		glDeleteProgram(m_id);
	}
}

void Shader::Bind()
{
	glUseProgram(m_id);
}

void Shader::Unbind()
{
	glUseProgram(0);
}

#define UNIFORM_LOCATION glGetUniformLocation(m_id, name.c_str())
#define SET_UNIFORM(Func) Func(UNIFORM_LOCATION, value)
#define SET_UNIFORM_KNOWN(Func) Func(uniform, value)

Shader::Uniform Shader::GetUniformLocation(std::string_view name) const
{
	return glGetUniformLocation(m_id, name.data());
}

Shader::Uniform Shader::GetUniformLocation(Shader::CachedUniform uniform) const
{
	return m_cachedUniforms[(uint8_t)uniform];
}

void Shader::SetBool(const std::string& name, bool value) const
{
	SET_UNIFORM(glUniform1i);
}

void Shader::SetBool(Shader::Uniform uniform, bool value) const
{
	SET_UNIFORM_KNOWN(glUniform1i);
}

void Shader::SetInt32(const std::string& name, int32_t value) const
{
	SET_UNIFORM(glUniform1i);
}

void Shader::SetInt32(Shader::Uniform uniform, int32_t value) const
{
	SET_UNIFORM_KNOWN(glUniform1i);
}

void Shader::SetFloat(const std::string& name, float value) const
{
	SET_UNIFORM(glUniform1f);
}

void Shader::SetFloat(Shader::Uniform uniform, float value) const
{
	SET_UNIFORM_KNOWN(glUniform1i);
}

void Shader::SetVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2f(UNIFORM_LOCATION, value.x, value.y);
}

void Shader::SetVec2(Shader::Uniform uniform, const glm::vec2& value) const
{
	glUniform2f(uniform, value.x, value.y);
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3f(UNIFORM_LOCATION, value.x, value.y, value.z);
}

void Shader::SetVec3(Shader::Uniform uniform, const glm::vec3& value) const
{
	glUniform3f(uniform, value.x, value.y, value.z);
}

void Shader::SetVec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4f(UNIFORM_LOCATION, value.x, value.y, value.z, value.w);
}

void Shader::SetVec4(Shader::Uniform uniform, const glm::vec4& value) const
{
	glUniform4f(uniform, value.x, value.y, value.z, value.w);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) const
{
	glUniformMatrix4fv(UNIFORM_LOCATION, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetMat4(Shader::Uniform uniform, const glm::mat4& value) const
{
	glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::CacheUniforms()
{
	m_cachedUniforms[(uint8)CachedUniform::Time] = GetUniformLocation("u_Time");
	m_cachedUniforms[(uint8)CachedUniform::ScreenResolution] = GetUniformLocation("u_ScreenResolution");
	m_cachedUniforms[(uint8)CachedUniform::View] = GetUniformLocation("u_View");
	m_cachedUniforms[(uint8)CachedUniform::Projection] = GetUniformLocation("u_Projection");
	m_cachedUniforms[(uint8)CachedUniform::Transform] = GetUniformLocation("u_Transform");
	m_cachedUniforms[(uint8)CachedUniform::MVP] = GetUniformLocation("u_MVP");
	m_cachedUniforms[(uint8)CachedUniform::ViewProjection] = GetUniformLocation("u_VP");
}
