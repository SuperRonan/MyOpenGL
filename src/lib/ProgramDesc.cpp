#include "ProgramDesc.h"
#include <iostream>
#include <cassert>

namespace lib
{
	ProgramDesc::ProgramDesc(ShaderDesc* vertex_shader, ShaderDesc* fragment_shader):
		m_vertex_shader(vertex_shader),
		m_fragment_shader(fragment_shader),
		m_id(0)
	{}

	ProgramDesc::ProgramDesc(ProgramDesc&& other) :
		m_vertex_shader(other.m_vertex_shader),
		m_fragment_shader(other.m_fragment_shader),
		m_id(other.m_id)
	{
		other.m_vertex_shader = nullptr;
		other.m_fragment_shader = nullptr;
		other.m_id = 0;
	}

	ProgramDesc::~ProgramDesc()
	{
		if (isLinked())
		{
			glDeleteProgram(m_id);
			m_id = 0;
		}
	}

	bool ProgramDesc::link()
	{
		m_id = glCreateProgram();
		ShaderDesc* shaders[2] = { m_vertex_shader, m_fragment_shader };
		for (int i = 0; i < 2; ++i)
		{
			ShaderDesc& shader = *shaders[i];
			glAttachShader(m_id, shader.id());
		}

		glLinkProgram(m_id);
		GLint sucess;
		glGetProgramiv(m_id, GL_LINK_STATUS, &sucess);
		if (!sucess)
		{
			const int N = 1024;
			GLchar log[N];
			glGetProgramInfoLog(m_id, N, NULL, log);
			std::cerr << "Error, could not link the program!\n" << log << std::endl;
			glDeleteProgram(m_id);
			m_id = 0;
			return false;
		}
		return true;
	}

	GLuint ProgramDesc::id()const
	{
		return m_id;
	}

	bool ProgramDesc::isLinked()const
	{
		return m_id != 0;
	}

	void ProgramDesc::use()const
	{
		assert(isLinked());
		glUseProgram(id());
	}

	void ProgramDesc::useNone()
	{
		glUseProgram(0);
	}

	bool ProgramDesc::isValidUniformId(GLint u_id)
	{
		return u_id != -1;
	}
}