#pragma once

#include "Math.h"
#include "ProgramDesc.h"

#include <memory>
#include <string>
#include <filesystem>

namespace lib
{
	class Material
	{
	protected:

		static std::filesystem::path s_shader_path;

	public:

		std::shared_ptr<ProgramDesc> m_program;

		Material() = default;
		Material(std::shared_ptr<ProgramDesc> const& program);
		Material(std::shared_ptr<ProgramDesc> && program);

		Material(Material const& other) = default;
		Material(Material && other) = default;


		virtual void use();

		template <class Float>
		void setMatrices(Matrix4<Float> const& M, Matrix4<Float> const& V, Matrix4<Float> const& P)
		{
			m_program->setUniform("u_M", M);
			m_program->setUniform("u_V", V);
			m_program->setUniform("u_P", P);
		}
		

	};



	template <class Float>
	class Phong : public Material
	{
	protected:
		static std::shared_ptr<ProgramDesc> s_phong_shader;

	public:

		using Vector3 = Vector3<Float>;
		using Vector4 = Vector4<Float>;


		
		Vector3 m_diffuse;
		// color + specular
		Vector4 m_glossy;
		
	public:

		Phong(Vector3 const& diffuse, Vector4 const& glossy) :
			Material(s_phong_shader),
			m_diffuse(diffuse),
			m_glossy(glossy)
		{
			if (!s_phong_shader->isLinked())
				s_phong_shader->link();
			s_phong_shader->printUniforms(std::cout);
		}



		virtual void use()
		{
			Material::use();
			m_program->setUniform("u_diffuse", m_diffuse);
			m_program->setUniform("u_glossy", m_glossy);
		}
	};
}