#include "Material.h"

namespace lib
{
	std::filesystem::path Material::s_shader_path = "../shaders/";

	
	std::shared_ptr<ProgramDesc> Phong<float>::s_phong_shader = std::make_shared<ProgramDesc>(Material::s_shader_path.string() + "phong");
	std::shared_ptr<ProgramDesc> Cartoon<float>::s_cartoon_shader = std::make_shared<ProgramDesc>(Material::s_shader_path.string() + "cartoon");
	

	Material::Material(std::shared_ptr<ProgramDesc> const& program):
		m_program(program)
	{
		if (!m_program->isLinked())
			m_program->link();
	}

	Material::Material(std::shared_ptr<ProgramDesc> && program) :
		m_program(program)
	{
		if (!m_program->isLinked())
			m_program->link();
	}

	void Material::use()
	{
		m_program->use();
	}

	std::filesystem::path const& Material::shaderPath()
	{
		return s_shader_path;
	}
}