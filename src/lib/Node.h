#pragma once

#include <memory>
#include <vector>
#include <functional>

#include "Mesh.h"
#include "Math.h"
#include "Material.h"

namespace lib
{
	template <class Float>
	struct Drawable
	{
	protected:

	public:

		std::shared_ptr<Mesh<Float>> mesh;
		std::shared_ptr<Material> material;

		Drawable(std::shared_ptr<Mesh<Float>> const& mesh, std::shared_ptr<Material> const& material) :
			mesh(mesh),
			material(material)
		{}
	};

	template <class Float>
	class Node
	{
	protected:

		using Matrix4 = Matrix4<Float>;
		using Drawable = Drawable<Float>;

	public:

		Matrix4 transform;

		std::vector<std::shared_ptr<Node>> sons;

		std::vector<Drawable> drawable;

		Node(Matrix4 const& mat = Matrix4(1)):
			transform(mat)
		{}

		Node(Matrix4 const& mat, std::vector<std::shared_ptr<Node>> const& sons):
			transform(mat),
			sons(sons)
		{}

		Node(Matrix4 const& mat, std::vector<std::shared_ptr<Node>> && sons) :
			transform(mat),
			sons(sons)
		{}

		void addDrawable(Drawable const& d)
		{
			drawable.push_back(d);
		}

		template <class... Args>
		void emplaceDrawable(Args&&... args)
		{
			drawable.emplace_back(args...);
		}
		
		virtual void update(Float t, Float dt)
		{}
	};

	template <class Float>
	class LambdaNode: public Node<Float>
	{
	protected:

		using Matrix4 = Matrix4<Float>;

		std::function<void(Float, Float, Matrix4&)> m_function;

	public:

		template <class Function>
		LambdaNode(Function const& f) :
			m_function(f)
		{}

		template <class Function>
		LambdaNode(Function && f) :
			m_function(f)
		{}

		virtual void update(Float t, Float dt) override
		{
			m_function(t, dt, Node<Float>::transform);
		}
	};
}