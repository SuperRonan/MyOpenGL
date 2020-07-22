#pragma once

#include "Math.h"

namespace lib
{
	template <class Float>
	struct Vertex
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


		static constexpr int stride()
		{
			return sizeof(Vertex);
		}

		static constexpr int positionOffset()
		{
			return offsetof(Vertex, m_position);
		}

		static constexpr int normalOffset()
		{
			return offsetof(Vertex, m_normal);
		}

		static constexpr int uvOffset()
		{
			return offsetof(Vertex, m_uv);
		}
	};
}