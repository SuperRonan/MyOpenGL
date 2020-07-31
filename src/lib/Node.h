#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <type_traits>

#include "Mesh.h"
#include "Math.h"
#include "Material.h"
#include "Component.h"

namespace lib
{
	template <class Float>
	class Node
	{
	protected:

		using Matrix4 = Matrix4<Float>;

	public:

		Matrix4 transform;

		std::vector<std::shared_ptr<Node>> sons;

		std::vector<std::shared_ptr<Component>> components;

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

		template<class ComponentDerived>
		void addNew(ComponentDerived const& c)
		{
			static_assert(std::is_base_of<Component, ComponentDerived>::value);
			components.emplace_back(std::make_shared<ComponentDerived>(c));
		}

		template<class ComponentDerived>
		void addNew(ComponentDerived && c)
		{
			static_assert(std::is_base_of<Component, ComponentDerived>::value);
			components.emplace_back(std::make_shared<ComponentDerived>(c));
		}

		template <class ComponentDerived, class... Args>
		void emplaceNew(Args&&... args)
		{
			static_assert(std::is_base_of<Component, ComponentDerived>::value);
			components.emplace_back(std::make_shared<ComponentDerived>(args...));
		}

		void add(std::shared_ptr<Component> c)
		{
			components.emplace_back(c);
		}
		

		std::shared_ptr<Component> getComponent(int i)
		{
			return components[i];
		}
		
		template <class ComponentDerived>
		std::shared_ptr<ComponentDerived> get()
		{
			static_assert(std::is_base_of<Component, ComponentDerived>::value);
			for (int i = 0; i < components.size(); ++i)
			{
				std::shared_ptr<ComponentDerived> res = std::dynamic_pointer_cast<ComponentDerived>(components[i]);
				if (res.get())
					return res;
			}
			return nullptr;
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