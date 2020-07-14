#pragma once

#include <glad/glad.h>
#include <string>


namespace lib
{
	class ShaderDesc
	{
	protected:

		GLenum m_type;
		
		std::string m_file;

		GLuint m_shader_id;

	public:

		ShaderDesc(const char* file, GLenum type);

		ShaderDesc(std::string const& file, GLenum type);

		ShaderDesc(ShaderDesc const&) = delete;

		ShaderDesc(ShaderDesc&& other);

		~ShaderDesc();
		
		bool compile();

		bool isCompiled()const;

		GLuint id()const;
	};
}