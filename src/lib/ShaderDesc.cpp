#include "ShaderDesc.h"
#include <iostream>
#include <streambuf>
#include <fstream>

namespace lib
{
    std::string readFile(const char* file_name)
    {
        std::string res;
        std::ifstream file(file_name);
        if (!file.is_open())
            throw std::runtime_error(std::string("Unable to open file ") + file_name);
        file.seekg(0, std::ios::end);
        res.reserve(file.tellg());
        file.seekg(0, std::ios::beg);
        res.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        file.close();
        return res;
    }

    std::string readFile(const std::string& file_name)
    {
        return readFile(file_name.c_str());
    }

    ShaderDesc::ShaderDesc():
        m_shader_id(0)
    {}

    ShaderDesc::ShaderDesc(const char* file, GLenum type) :
        m_file(file),
        m_type(type),
        m_shader_id(0)
	{}

    ShaderDesc::ShaderDesc(const std::string& file, GLenum type) :
        m_file(file),
        m_type(type),
        m_shader_id(0)
    {}

    ShaderDesc::ShaderDesc(ShaderDesc&& other) :
        m_file(std::move(other.m_file)),
        m_type(other.m_type),
        m_shader_id(other.id())
    {
        other.m_shader_id = 0;
    }

    ShaderDesc::~ShaderDesc()
    {
        if (id())
        {
            glDeleteShader(m_shader_id);
            m_shader_id = 0;
        }
    }

    ShaderDesc& ShaderDesc::operator=(ShaderDesc&& other)
    {
        m_file = std::move(other.m_file);
        m_type = other.m_type;
        m_shader_id = other.m_shader_id;
        other.m_shader_id = 0;
        return *this;
    }

	bool ShaderDesc::compile(std::vector<std::string> const& defines)
	{
        std::string code = readFile(m_file);

        if (!defines.empty())
        {
            // remove the #version at the begining of the file
            auto ptr_1 = std::find(code.begin(), code.end(), '#');
            auto ptr_2 = std::find(code.begin(), code.end(), '\n');
            std::string header(ptr_1, ptr_2);
            std::fill(code.begin(), ptr_2, ' ');
            
            for (std::string const& define : defines)
            {
                header += std::string("#define ") + define + "\n";
            }
            code = header + code;
        }

        const GLchar* code_gl = code.c_str();

        m_shader_id = glCreateShader(m_type);
        glShaderSource(m_shader_id, 1, &code_gl, NULL);
        glCompileShader(m_shader_id);

        GLint sucess;
        glGetShaderiv(m_shader_id, GL_COMPILE_STATUS, &sucess);
        if (!sucess)
        {
            const int N = 1024;
            GLchar log[N];
            glGetShaderInfoLog(m_shader_id, N, NULL, log);
            std::cerr << "Error, could not compile the shader " << m_file << "\n" << log << std::endl;
            glDeleteShader(m_shader_id);
            m_shader_id = 0;
            return false;
        }
        return true;
	}


    bool ShaderDesc::isCompiled()const
    {
        return id() != 0;
    }
    

    GLuint ShaderDesc::id()const
    {
        return m_shader_id;
    }
}