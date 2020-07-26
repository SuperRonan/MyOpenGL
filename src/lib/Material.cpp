#include "Material.h"

namespace lib
{
	std::filesystem::path Material::s_shader_path = "../shaders/";

	std::shared_ptr<ProgramDesc> Phong<float>::s_phong_shader = std::make_shared<ProgramDesc>(Material::s_shader_path.string() + "phong");

	Material::Material(std::shared_ptr<ProgramDesc> const& program):
		m_program(program)
	{}

	Material::Material(std::shared_ptr<ProgramDesc> && program) :
		m_program(program)
	{}

	void Material::use()
	{
		m_program->use();
	}
}