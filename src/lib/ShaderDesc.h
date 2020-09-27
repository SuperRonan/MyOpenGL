#pragma once

#include <glad/glad.h>
#include <string>
#include <vector>


namespace lib
{
	class ShaderDesc
	{
	protected:

		GLenum m_type;
		
		std::string m_file;

		GLuint m_shader_id;

	public:

		ShaderDesc();

		ShaderDesc(const char* file, GLenum type);

		ShaderDesc(std::string const& file, GLenum type);

		ShaderDesc(ShaderDesc const&) = delete;

		ShaderDesc(ShaderDesc&& other);

		ShaderDesc& operator=(ShaderDesc&& other);

		~ShaderDesc();
		
		bool compile(std::vector<std::string> const& defines=std::vector<std::string>());

		bool isCompiled()const;

		GLuint id()const;
	};
}