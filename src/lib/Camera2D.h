#pragma once

#include <glm/glm.hpp>
#include "Transforms.h"

namespace lib
{
	template <class Float>
	class Camera2D
	{
	protected:

		// Screen space: something like [0, 1[ x [0, 1[ (not centered in zero)

		using Vector2 = Vector2<Float>;
		using Vector3 = Vector3<Float>;
		using Matrix3 = Matrix3<Float>;

		float m_ds = 0.1f;

		Vector2 m_translate;

		Float m_zoom;

		//Screen space to camera window
		Matrix3 m_zoom_matrix;

	public:

		Camera2D() :
			m_translate(0.f, 0.f),
			m_zoom(1.f),
			m_zoom_matrix(1.f)
		{}

		// pos in screen space
		Vector2 operator()(Vector2 const& pos)const
		{
			return m_translate + lib::deHomogenize<2, Float>(m_zoom_matrix * lib::homogenize<2, Float>(pos));
		}

		Matrix3 matrix()const
		{
			return lib::translateMatrix<3, Float>(m_translate) * m_zoom_matrix;
		}

		void move(Vector2 const& screen_delta)
		{
			m_translate -= screen_delta * m_zoom;
		}

		// Assumes scroll is not 0
		void zoom(Vector2 const& screen_pos, Float scroll)
		{
			assert(scroll != 0);
			
			Vector2 camera_pos = lib::deHomogenize<2, Float>(m_zoom_matrix * lib::homogenize<2, Float>(screen_pos));
			
			Float mult = scroll > 0 ? (1.0f / (1.0f + scroll * m_ds)) : ((-scroll * m_ds + 1.0f));

			m_zoom *= mult;

			Vector2 compensate_vector = camera_pos * (1 - mult);
			Matrix3 compensate_matrix = lib::translateMatrix<3, Float>(compensate_vector);

			Matrix3 mult_matrix = lib::scaleMatrix<3, Float>({ mult, mult });

			m_zoom_matrix = compensate_matrix * mult_matrix * m_zoom_matrix;
		}

		void reset()
		{
			m_translate = { 0, 0 };
			m_zoom = 1;
			m_zoom_matrix = Matrix3(1);
		}
	};
}