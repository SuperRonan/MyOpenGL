#pragma once

#include "Math.h"
#include "ProgramDesc.h"
#include "Texture.h"

#include <memory>
#include <string>
#include <filesystem>
#include <map>
#include <cmath>

namespace lib
{
	class Material
	{
	protected:

		static std::filesystem::path s_shader_path;

	public:

		std::shared_ptr<ProgramDesc> m_program;

		bool uses_lighting = false;

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
		
		static std::filesystem::path const& shaderPath();
	};



	template <class Float>
	class Phong : public Material
	{
	protected:

		static std::shared_ptr<ProgramDesc> s_phong_shader;

		static int shaderIndex(bool has_diffuse_tex)
		{
			int res = 0;
			if (has_diffuse_tex)	res += 1;

			return res;
		}

		using Vector3 = Vector3<Float>;
		using Vector4 = Vector4<Float>;

		
	public:
		std::shared_ptr<Texture> m_diffuse_tex;
		Vector3 m_diffuse;
		
		// color + specular
		Vector4 m_glossy;

		Vector3 m_emissive;

		// RGB = normal
		// A = depth / height
		std::shared_ptr<Texture> m_nh_map;
		
	public:

		Phong(Vector3 const& diffuse = { 1, 1, 1 }, Vector4 const& glossy = { 0, 0, 0, 1 }, Vector3 const& emissive = { 0, 0, 0 }) :
			Material(s_phong_shader),
			m_diffuse_tex(nullptr),
			m_diffuse(diffuse),
			m_glossy(glossy),
			m_emissive(emissive),
			m_nh_map(nullptr)
		{
			if (!s_phong_shader->isLinked())
				s_phong_shader->link();
			s_phong_shader->printUniforms(std::cout);
			uses_lighting = true;
		}



		virtual void use()
		{
			Material::use();
			int tex_flags = 0;
			if (m_diffuse_tex.get() && m_diffuse_tex->deviceValid())
			{
				m_diffuse_tex->use(0);
				tex_flags += 1;
				m_program->setUniform("u_t_diffuse", 0);
			}

			m_program->setUniform("u_c_diffuse", m_diffuse);
			m_program->setUniform("u_c_glossy", m_glossy);
			m_program->setUniform("u_c_emissive", m_emissive);

			if (m_nh_map.get() && m_nh_map->deviceValid())
			{
				m_nh_map->use(1);
				tex_flags += 8;
				m_program->setUniform("u_t_nh", 1);
			}

			m_program->setUniform("u_tex_flags", tex_flags);
		}
	};

	template <class Float>
	class Cartoon : public Material
	{
	protected:

		static std::shared_ptr<ProgramDesc> s_cartoon_shader;

		static int shaderIndex(bool has_diffuse_tex)
		{
			int res = 0;
			if (has_diffuse_tex)	res += 1;

			return res;
		}

		using Vector3 = Vector3<Float>;
		using Vector4 = Vector4<Float>;


	public:
		std::shared_ptr<Texture> m_diffuse_tex;
		Vector3 m_diffuse;

		// color + specular
		Vector4 m_glossy;

		Vector3 m_emissive;

		// RGB = normal
		// A = depth / height
		std::shared_ptr<Texture> m_nh_map;

	public:

		Cartoon(Vector3 const& diffuse = { 1, 1, 1 }, Vector4 const& glossy = { 0, 0, 0, 1 }, Vector3 const& emissive = { 0, 0, 0 }) :
			Material(s_cartoon_shader),
			m_diffuse_tex(nullptr),
			m_diffuse(diffuse),
			m_glossy(glossy),
			m_emissive(emissive),
			m_nh_map(nullptr)
		{
			if (!s_cartoon_shader->isLinked())
				s_cartoon_shader->link();
			s_cartoon_shader->printUniforms(std::cout);
			uses_lighting = true;
		}



		virtual void use()
		{
			Material::use();
			int tex_flags = 0;
			if (m_diffuse_tex.get() && m_diffuse_tex->deviceValid())
			{
				m_diffuse_tex->use(0);
				tex_flags += 1;
				m_program->setUniform("u_t_diffuse", 0);
			}

			m_program->setUniform("u_c_diffuse", m_diffuse);
			m_program->setUniform("u_c_glossy", m_glossy);
			m_program->setUniform("u_c_emissive", m_emissive);

			if (m_nh_map.get() && m_nh_map->deviceValid())
			{
				m_nh_map->use(1);
				tex_flags += 8;
				m_program->setUniform("u_t_nh", 1);
			}

			m_program->setUniform("u_tex_flags", tex_flags);
		}
	};
}