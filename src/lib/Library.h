#pragma once

#include <vector>

namespace lib
{
	template <class T>
	class Library
	{
	protected:

		std::vector<T> m_elements;

	public:

		Library()
		{}

		Library(Library const&) = delete;

		Library(Library&&) = delete;

		T const& operator[](int i)const
		{
			return m_elements[i];
		}

		T & operator[](int i)
		{
			return m_elements[i];
		}

		int add(T&& element)
		{
			m_elements.emplace_back(std::move(element));
			return m_elements.size
		}

		
	};
}