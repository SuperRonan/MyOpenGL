#pragma once

#include <memory>

#include "Math.h"
#include "Mesh.h"
#include "Camera.h"
#include "Material.h"
#include "Lights.h"
#include "Node.h"

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

		std::vector<Light> m_lights;

	public:


		Scene():
			m_lights(5, Light::NoneLight())
		{}

		Scene(Scene const&) = delete;
		Scene(Scene &&) = delete;


		void draw()
		{
			draw(base.transform, &base);
			ProgramDesc::useNone();
		}

	protected:

		void draw(Matrix4 const& matrix, Node* node)
		{
			for (Drawable& d : node->drawable)
			{
				d.material->use();
				d.material->setMatrices(matrix, m_camera.getMatrixV(), m_camera.getMatrixP());
				setLighting(m_lights, *d.material->m_program.get());

				//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

				d.mesh->draw();
			}
			for (std::shared_ptr<Node>& son : node->sons)
			{
				Matrix4 next = matrix * son->transform;
				draw(next, son.get());
			}
		}

		// assumes the program is already being used
		void setLighting(std::vector<Light> const& lights, ProgramDesc& program)
		{
			GLchar type_name[] = "u_lights[i].type";
			GLchar position_name[] = "u_lights[i].position";
			GLchar Le_name[] = "u_lights[i].Le";
			for (int i = 0; i < lights.size(); ++i)
			{
				assert(i <= 9);
				type_name[9] = '0' + i;
				position_name[9] = '0' + i;
				Le_name[9] = '0' + i;

				program.setUniform(type_name, (GLint)lights[i].type);
				program.setUniform(position_name, lights[i].position);
				program.setUniform(Le_name, lights[i].Le);
			}

			Vector3<Float> cam_pos = m_camera.getPosition();
			program.setUniform("u_w_camera_position", cam_pos);
		}

	};
}