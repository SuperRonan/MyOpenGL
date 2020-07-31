#pragma once

#include <memory>

#include "Math.h"
#include "Mesh.h"
#include "Camera.h"
#include "Material.h"
#include "Light.h"
#include "Node.h"
#include "Drawable.h"

namespace lib
{
	template <class Float>
	class Scene
	{
	public:

		using Matrix4 = Matrix4<Float>;
		using Mesh = Mesh<Float>;
		using Light = Light<Float>;
		using Node = Node<Float>;
		using Drawable = Drawable<Float>;

		Node base;
		
		Camera<Float> m_camera;

		using LightBuffer = std::vector<Light>;
		mutable LightBuffer m_lights_buffer;

		Vector3<Float> m_ambiant;



	public:


		Scene(Vector3<Float> ambiant = { 0, 0, 0 }) :
			m_lights_buffer(5, Light::NoneLight()),
			m_ambiant(ambiant)
		{}

		Scene(Scene const&) = delete;
		Scene(Scene &&) = delete;


		void draw()
		{
			std::fill(m_lights_buffer.begin(), m_lights_buffer.end(), Light::NoneLight());
			int n_lights = 0;
			buildLights(m_lights_buffer, n_lights, base.transform, &base);
			draw(base.transform, &base, m_lights_buffer);
			ProgramDesc::useNone();
		}

		void customDraw(Material* custom)
		{
			customDraw(base.transform, &base, custom);
			ProgramDesc::useNone();
		}

		void update(Float t, Float dt)
		{
			Node* node = &base;
			update(node, t, dt);
		}

	protected:

		void buildLights(LightBuffer & res, int & i, Matrix4 const& mat, Node * node)
		{
			std::shared_ptr<const Light> light_ptr = node->get<Light>();
			if (light_ptr.get())
			{
				assert(i <= res.size());
				Light light = light_ptr->transform(mat);
				res[i] = light;
				++i;
			}

			for (std::shared_ptr<Node>& son : node->sons)
			{
				Matrix4 next = mat * son->transform;
				buildLights(res, i, next, son.get());
			}	
		}

		void draw(Matrix4 const& matrix, Node* node, LightBuffer const& lights)
		{
			std::shared_ptr<Drawable> d_ptr = node->get<Drawable>();
			if(d_ptr)
			{
				Drawable& d = *d_ptr;
				d.material->use();
				d.material->setMatrices(matrix, m_camera.getMatrixV(), m_camera.getMatrixP());
				setLighting(lights, *d.material->m_program.get());

				//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

				d.mesh->draw();
			}
			for (std::shared_ptr<Node>& son : node->sons)
			{
				Matrix4 next = matrix * son->transform;
				draw(next, son.get(), lights);
			}
		}

		void customDraw(Matrix4 const& matrix, Node* node, Material * custom)
		{
			std::shared_ptr<Drawable> d_ptr = node->get<Drawable>();
			if (d_ptr)
			{
				custom->use();
				custom->setMatrices(matrix, m_camera.getMatrixV(), m_camera.getMatrixP());
				//setLighting(m_lights, *d.material->m_program.get());

				//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

				d_ptr->mesh->draw();
			}
			for (std::shared_ptr<Node>& son : node->sons)
			{
				Matrix4 next = matrix * son->transform;
				customDraw(next, son.get(), custom);
			}
		}

		// assumes the program is already being used
		void setLighting(LightBuffer const& lights, ProgramDesc& program)
		{
			GLchar type_name[] = "u_lights[i].type";
			GLchar position_name[] = "u_lights[i].position";
			GLchar direction_name[] = "u_lights[i].direction";
			GLchar Le_name[] = "u_lights[i].Le";
			for (int i = 0; i < lights.size(); ++i)
			{
				assert(i <= 9);
				type_name[9] = '0' + i;
				position_name[9] = '0' + i;
				direction_name[9] = '0' + i;
				Le_name[9] = '0' + i;

				program.setUniform(type_name, (GLint)lights[i].type);
				program.setUniform(position_name, lights[i].position);
				program.setUniform(direction_name, lights[i].direction);
				program.setUniform(Le_name, lights[i].Le);
			}

			Vector3<Float> cam_pos = m_camera.getPosition();
			program.setUniform("u_w_camera_position", cam_pos);
			program.setUniform("u_ambiant", m_ambiant);
		}

		void update(Node* node, Float t, Float dt)
		{
			node->update(t, dt);
			for (std::shared_ptr<Node>& son : node->sons)
			{	
				update(son.get(), t, dt);
			}
		}

	};
}