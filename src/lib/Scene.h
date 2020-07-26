#pragma once

#include <memory>

#include "Math.h"
#include "Mesh.h"
#include "Camera.h"
#include "Material.h"
#include "Lights.h"

namespace lib
{
	template <class Float>
	class Scene
	{
	public:

		using Matrix4 = Matrix4<Float>;
		using Mesh = Mesh<Float>;
		using Light = Light<Float>;

		struct Object
		{
			std::shared_ptr<Mesh> mesh;
			std::shared_ptr<Material> material;

			Object(std::shared_ptr<Mesh>& m, std::shared_ptr<Material>& mat):
				mesh(m),
				material(mat)
			{}

			Object(std::shared_ptr<Mesh>&& m, std::shared_ptr<Material>&& mat) :
				mesh(m),
				material(mat)
			{}
		};

		struct Node
		{
			Matrix4 transform;
			
			std::vector<std::shared_ptr<Node>> m_sons;

			std::vector<std::shared_ptr<Object>> m_objects;

			Node(Matrix4 transform = Matrix4(Float(1))):
				transform(transform)
			{}
		};

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
			for (std::shared_ptr<Object>& pobject : node->m_objects)
			{
				Object& object = *pobject;
				object.material->use();
				object.material->setMatrices(matrix, m_camera.getMatrixV(), m_camera.getMatrixP());
				setLights(m_lights, *object.material->m_program.get());
				object.mesh->draw();
			}
			Matrix4 next = node->transform * matrix;
			for (std::shared_ptr<Node>& son : node->m_sons)
			{
				draw(next, son.get());
			}
		}

		// assumes the program is already being used
		void setLights(std::vector<Light> const& lights, ProgramDesc& program)
		{
			for (int i = 0; i < lights.size(); ++i)
			{
				GLchar type_name[] = "u_lights[i].type";
				type_name[9] = '0' + i;
				GLchar position_name[] = "u_lights[i].position";
				position_name[9] = '0' + i;
				GLchar Le_name[] = "u_lights[i].Le";
				Le_name[9] = '0' + i;

				GLint type_location = glGetUniformLocation(program.id(), type_name);
				GLint position_location = glGetUniformLocation(program.id(), position_name);
				GLint Le_location = glGetUniformLocation(program.id(), Le_name);

				program.setUniform(type_location, (GLint)lights[i].type);
				program.setUniform(position_location, lights[i].position);
				program.setUniform(Le_location, lights[i].Le);
			}
		}

	};
}