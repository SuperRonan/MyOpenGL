#pragma once

#include <vector>
#include <memory>

namespace lib
{
	template <class T>
	class Library
	{
	protected:

		using SPT = std::shared_ptr<T>;
		
		std::vector<SPT> m_elements;

	public:

		Library()
		{}

		Library(Library const&) = delete;

		Library(Library&&) = delete;

		SPT const& operator[](int i)const
		{
			return m_elements[i];
		}

		SPT & operator[](int i)
		{
			return m_elements[i];
		}

		int add(T&& element)
		{
			m_elements.emplace_back(std::make_shared<T>(element));
			return m_elements.size();
		}



		
	};
}