#pragma once

#include "ShaderDesc.h"
#include <glad/glad.h>
#include <type_traits>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace lib
{
	class ProgramDesc
	{
	protected:

		ShaderDesc* m_vertex_shader;
		ShaderDesc* m_fragment_shader;

		GLuint m_id;

	public:

		ProgramDesc(ShaderDesc* vertex_shader, ShaderDesc* fragment_shader);

		ProgramDesc(ProgramDesc const&) = delete;

		ProgramDesc(ProgramDesc&& other);

		~ProgramDesc();

		bool link();

		GLuint id()const;

		bool isLinked()const;

		void use()const;

		static void useNone();

		static bool isValidUniformId(GLint u_id);

		template <class T>
		bool setUniform(GLint u_id, T const& value)const
		{
			if constexpr (std::is_same<bool, T>::value || std::is_same<int, T>::value)
			{
				glUniform1i(u_id, (GLint)value);
			}
			else if constexpr (std::is_same<float, T>::value)
			{
				glUniform1f(u_id, value);
			}
			else if constexpr (std::is_same<glm::vec2, T>::value)
			{
				glUniform2fv(u_id, 1, glm::value_ptr(value));
			}
			else if constexpr (std::is_same<glm::vec3, T>::value)
			{
				glUniform3fv(u_id, 1, glm::value_ptr(value));
			}
			else if constexpr (std::is_same<glm::vec4, T>::value)
			{
				glUniform4fv(u_id, 1, glm::value_ptr(value));
			}
			else if constexpr (std::is_same<glm::mat3x3, T>::value)
			{
				glUniformMatrix3fv(u_id, 1, GL_FALSE, glm::value_ptr(value));
			}
			else if constexpr (std::is_same<glm::mat4x4, T>::value)
			{
				glUniformMatrix4fv(u_id, 1, GL_FALSE, glm::value_ptr(value));
			}
			else if constexpr (std::is_same<double, T>::value)
			{
				glUniform1d(u_id, value);
			}
			else if constexpr (std::is_same<glm::vec<2, double>, T>::value)
			{
				glUniform2dv(u_id, 1, glm::value_ptr(value));
			}
			else if constexpr (std::is_same<glm::vec<3, double>, T>::value)
			{
				glUniform3dv(u_id, 1, glm::value_ptr(value));
			}
			else if constexpr (std::is_same<glm::vec<4, double>, T>::value)
			{
				glUniform4dv(u_id, 1, glm::value_ptr(value));
			}
			else if constexpr (std::is_same<glm::mat<3, 3, double>, T>::value)
			{
				std::cout << glUniformMatrix3dv << std::endl;
				glUniformMatrix3dv(u_id, 1, GL_FALSE, glm::value_ptr(value));
			}
			else if constexpr (std::is_same<glm::mat<4, 4, double>, T>::value)
			{
				glUniformMatrix4dv(u_id, 1, GL_FALSE, glm::value_ptr(value));
			}
			else if constexpr (std::is_same<glm::uvec2, T>::value)
			{
				glUniform2uiv(u_id, 1, glm::value_ptr(value));
			}
			else
			{
				// Type T is not recognized
				return false;
			}
			return true;
		}

		template <class T>
		bool setUniform(const char* name, T const& value)const
		{
			GLint u_id = glGetUniformLocation(id(), name);
			if (isValidUniformId(u_id))
			{
				setUniform<T>(u_id, value);
				return true;
			}
			// Uniform is not recognized
			// should shout a warning
			return false;
		}

		template <class T>
		__forceinline bool setUniform(const std::string& name, T const& value)const
		{
			return setUniform(name.c_str(), value);
		}

		template <class Out>
		Out& printUniforms(Out& out)const
		{
			GLint N;
			glGetProgramiv(id(), GL_ACTIVE_UNIFORMS, &N);
			out << N << " uniforms\n";
			char name[256];
			for (GLuint i = 0; i < N; ++i)
			{
				GLint size;
				GLenum type;
				glGetActiveUniform(id(), i, 256, NULL, &size, &type, name);
				out << name << '\n';
			}
			return out;
		}

		template <class Out>
		Out& printAttributes(Out& out)const
		{
			GLint N;
			glGetProgramiv(id(), GL_ACTIVE_ATTRIBUTES, &N);
			out << N << " attributes\n";
			char name[256];
			for (GLuint i = 0; i < N; ++i)
			{
				GLint size;
				GLenum type;
				glGetActiveAttrib(id(), i, 256, NULL, &size, &type, name);
				out << name << '\n';
			}
			return out;
		}
	};
}