#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include "Math.h"

namespace lib
{
	template <class Float>
	class Camera
	{
	protected:


		Vector3<Float> m_position;
		Vector3<Float> m_front;
		Vector3<Float> m_up;
		Vector3<Float> m_right;

		__forceinline void updateRight()
		{
			m_right = glm::normalize(glm::cross(m_front, m_up));
		}

	public:

		Camera(Vector3<Float> const& position = { Float(0), Float(0), Float(0) }, Vector3<Float> const& front = { Float(0), Float(0), Float(-1) }) :
			m_position(position),
			m_front(glm::normalize(front)),
			m_up(Float(0), Float(1), Float(0)),
			m_right(glm::cross(m_front, m_up))
		{}
		
		Camera(Camera const& other) = default;

		Matrix4<Float> getMatrix()const
		{
			return glm::lookAt(m_position, m_position + m_front, m_up);
		}

		void setTarget(Vector3<Float> const& target)
		{
			m_front = glm::normalize(target - m_position);
			updateRight();
		}

		// assumes dir is normalized
		void setDirection(Vector3<Float> const& dir)
		{
			m_front = dir;
			updateRight();
		}

		void move(Vector3<Float> const& movement)
		{
			m_position += movement.x * m_right + movement.y * m_front + movement.z * m_up;
		}

	};
}