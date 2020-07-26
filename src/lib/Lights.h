#pragma once

#include "Math.h"
#include "ProgramDesc.h"

namespace lib
{
	template <typename Float>
	struct Light
	{
		using Vector3 = Vector3<Float>;

		enum class Type : int { None = 0, Point = 1 };

		Type type;

		Vector3 position;

		Vector3 Le;


		Light(Vector3 const& position, Vector3 const& Le, Type type):
			position(position),
			Le(Le),
			type(type)
		{}

		static Light PointLight(Vector3 const& position, Vector3 const& Le)
		{
			return Light(position, Le, Type::Point);
		}

		static Light NoneLight()
		{
			return Light(Vector3(0), Vector3(0), Type::None);
		}

		void setUniforms(ProgramDesc* program, int offset)const
		{
			
		}
	};
}