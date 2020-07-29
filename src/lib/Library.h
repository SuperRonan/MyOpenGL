#pragma once

#include <unordered_map>
#include <memory>
#include <string>

namespace lib
{
	template <class T>
	class Library
	{
	protected:

		using SPT = std::shared_ptr<T>;
		// use a view string or something
		std::unordered_map<std::string, SPT> m_elements;

	public:

		Library()
		{}

		Library(Library const&) = delete;

		Library(Library&&) = default;

		SPT const& operator[](std::string const& key)const
		{
			return m_elements.at(key);
		}

		SPT & operator[](std::string const& key)
		{
			return m_elements[key];
		}

		SPT& add(std::string const& key, T&& element)
		{
			return m_elements[key] = std::make_shared<T>(element);
		}

		SPT& add(std::string const& key, SPT elem)
		{
			return m_elements[key] = elem;
		}

		SPT& add(std::string && key, T&& element)
		{
			return m_elements[key] = std::make_shared<T>(element);
		}

		SPT& add(std::string && key, SPT elem)
		{
			return m_elements[key] = elem;
		}

		
		
	};
}