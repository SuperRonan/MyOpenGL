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

		template <class Q>
		SPT& addBase(std::string const& key, Q&& base)
		{
			return m_elements[key] = std::make_shared<T>(std::move(base));
		}

		template <class Q>
		SPT& addDerived(std::string const& key, Q&& derived)
		{
			return m_elements[key] = std::make_shared<Q>(std::move(derived));
		}

		SPT& add(std::string const& key, SPT elem)
		{
			return m_elements[key] = elem;
		}

		//template <class Q>
		//SPT& add(std::string && key, Q&& element)
		//{
		//	return m_elements[std::move(key)] = std::make_shared<Q>(std::move(element));
		//}

		//SPT& add(std::string && key, SPT elem)
		//{
		//	return m_elements[key] = elem;
		//}

		template <class Q, class... Args>
		SPT& emplace(std::string const& key, Args&&... args)
		{
			return m_elements[key] = std::make_shared<Q>(args...);
		}

		//template <class Q, class... Args>
		//SPT& emplace(std::string && key, Args&&... args)
		//{
		//	return m_elements[std::move(key)] = std::make_shared<Q>(args...);
		//}
		
	};
}