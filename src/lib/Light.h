#pragma once

#include "Math.h"
#include "Transforms.h"
#include "ProgramDesc.h"
#include "Component.h"

namespace lib
{
	template <typename Float>
	struct Light: public Component 
	{
		using Vector3 = Vector3<Float>;
		using Matrix4 = Matrix4<Float>;

		enum class Type : int { None = 0, Point = 1, Directional = 2 };

		Type type;

		Vector3 position;
		// direction in which the radiance is emitted
		Vector3 direction;

		Vector3 Le;


		Light(Vector3 const& position = { 0, 0, 0 }, Vector3 const& direction = { 0, -1, 0 }, Vector3 const& Le = { 0, 0, 0 }, Type type = Type::None) :
			position(position),
			direction(direction),
			Le(Le),
			type(type)
		{}

		Light(Light const& other) = default;

		virtual ~Light() = default;

		static Light PointLight(Vector3 const& position, Vector3 const& Le)
		{
			return Light(position, Vector3(0), Le, Type::Point);
		}

		static Light DirectionalLight(Vector3 const& direction, Vector3 const& Le)
		{
			return Light(Vector3(0), direction, Le, Type::Directional);
		}

		static Light NoneLight()
		{
			return Light(Vector3(0), Vector3(0), Vector3(0), Type::None);
		}

		Light transform(Matrix4 const& mat)const
		{
			Light res = *this;
			res.position = deHomogenize<3, Float>(mat * homogenize<3, Float>(position));
			res.direction = directionMatrix(mat) * direction;
			return res;
		}
	};
}