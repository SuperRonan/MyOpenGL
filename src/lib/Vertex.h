#pragma once

#include "Math.h"

namespace lib
{
	template <class Float>
	struct Vertex
	{
	public:

		Vector3<Float> position;
		Vector3<Float> normal;
		Vector3<Float> tangent;
		Vector2<Float> uv;


	public:

		Vertex() = default;

		Vertex(Vector3<Float> const& position, Vector3<Float> const& normal, Vector3<Float> const& tangent, Vector2<Float> const& uv) :
			position(position),
			normal(normal),
			tangent(tangent),
			uv(uv)
		{}

		Vertex(Vector3<Float> const& position, Vector3<Float> const& normal, Vector2<Float> const& uv) :
			position(position),
			normal(normal),
			tangent(0),
			uv(uv)
		{}

		Vertex(Vertex const& other) = default;


		static constexpr int stride()
		{
			return sizeof(Vertex);
		}

		static constexpr int positionOffset()
		{
			return offsetof(Vertex, position);
		}

		static constexpr int normalOffset()
		{
			return offsetof(Vertex, normal);
		}

		static constexpr int tangentOffset()
		{
			return offsetof(Vertex, tangent);
		}

		static constexpr int uvOffset()
		{
			return offsetof(Vertex, uv);
		}
	};
}