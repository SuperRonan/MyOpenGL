#pragma once

#include "Math.h"

namespace lib
{
	template <class Float>
	class Vertex
	{
	public:

		Vector3<Float> m_position;
		Vector3<Float> m_normal;
		Vector2<Float> m_uv;


	public:

		Vertex(Vector3<Float> const& position, Vector3<Float> const& normal, Vector2<Float> const& uv) :
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