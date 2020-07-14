#pragma once

#include <glm/glm.hpp>

namespace lib
{
	template <class Float>
	class Vertex
	{
	public:

		using Vector3 = glm::vec<3, Float>;
		using Vector2 = glm::vec<2, Float>;

		Vector3 m_position;
		Vector3 m_normal;
		Vector2 m_uv;


	public:

		Vertex(Vector3 const& position, Vector3 const& normal, Vector2 const& uv) :
			m_position(position),
			m_normal(normal),
			m_uv(uv)
		{}

		Vertex(Vertex const& other) = default;


		static int stride()
		{
			return sizeof(Vertex);
		}

		static int positionOffset()
		{
			return 0;
		}

		static int normalOffset()
		{
			return sizeof(m_position);
		}

		static int uvOffset()
		{
			return sizeof(m_position) + sizeof(m_normal);
		}
	};
}