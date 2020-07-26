#pragma once

#include <memory>

#include "Math.h"
#include "Mesh.h"
#include "Camera.h"
#include "Material.h"

namespace lib
{
	template <class Float>
	class Scene
	{
	public:

		using Matrix4 = Matrix4<Float>;
		using Mesh = Mesh<Float>;

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



	public:


		Scene()
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

				object.mesh->draw();
			}
			Matrix4 next = node->transform * matrix;
			for (std::shared_ptr<Node>& son : node->m_sons)
			{
				draw(next, son.get());
			}
		}

	};
}