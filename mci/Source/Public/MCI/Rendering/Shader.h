#pragma once

#include "MCI/Common.h"
#include <unordered_map>
#include <array>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace MCI
{

	class Shader final
	{
	public:

        using Uniform = uint32;

        enum class Type
        {
            Vertex, Fragment, Geometry
        };

        enum class CachedUniform
        {
            Time, ScreenResolution, View, Projection, Transform, MVP, ViewProjection, Max
        };

        Shader(const std::unordered_map<Type, std::string_view>& sources, const std::unordered_map<std::string_view, std::string>& parameters = std::unordered_map<std::string_view, std::string>());
        ~Shader();

        void Bind();
        void Unbind();

        Uniform GetUniformLocation(std::string_view name) const;
        Uniform GetUniformLocation(CachedUniform uniform) const;

        void SetBool(const std::string& name, bool value) const;
        void SetBool(Uniform uniform, bool value) const;
        void SetInt32(const std::string& name, int32_t value) const;
        void SetInt32(Uniform uniform, int32_t value) const;
        void SetFloat(const std::string& name, float value) const;
        void SetFloat(Uniform uniform, float value) const;
        void SetVec2(const std::string& name, const glm::vec2& value) const;
        void SetVec2(Uniform uniform, const glm::vec2& value) const;
        void SetVec3(const std::string& name, const glm::vec3& value) const;
        void SetVec3(Uniform uniform, const glm::vec3& value) const;
        void SetVec4(const std::string& name, const glm::vec4& value) const;
        void SetVec4(Uniform uniform, const glm::vec4& value) const;
        void SetMat4(const std::string& name, const glm::mat4& value) const;
        void SetMat4(Uniform uniform, const glm::mat4& value) const;

        bool IsValid() const { return m_id != 0; }

    private:

        uint32_t m_id;

        std::array<Uniform, (std::size_t)CachedUniform::Max> m_cachedUniforms;

        void CacheUniforms();

	};

}