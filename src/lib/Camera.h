#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace lib
{
	template <class Float>
	class Camera
	{
	protected:

		using Vector3 = glm::vec<3, Float>;
		using Vector2 = glm::vec<2, Float>;
		using Matrix4 = glm::mat<4, 4, Float>;

		Vector3 m_position;
		Vector3 m_front;
		Vector3 m_up;
		Vector3 m_right;

		__forceinline void updateRight()
		{
			m_right = glm::normalize(glm::cross(m_front, m_up));
		}

	public:

		Camera(Vector3 const& position = { Float(0), Float(0), Float(0) }, Vector3 const& front = { Float(0), Float(0), Float(-1) }) :
			m_position(position),
			m_front(glm::normalize(front)),
			m_up(Float(0), Float(1), Float(0)),
			m_right(glm::cross(m_front, m_up))
		{}
		
		Camera(Camera const& other) = default;

		Matrix4 getMatrix()const
		{
			return glm::lookAt(m_position, m_position + m_front, m_up);
		}

		void setTarget(Vector3 const& target)
		{
			m_front = glm::normalize(target - m_position);
			updateRight();
		}

		// assumes dir is normalized
		void setDirection(Vector3 const& dir)
		{
			m_front = dir;
			updateRight();
		}

		void move(Vector3 const& movement)
		{
			m_position += movement.x * m_right + movement.y * m_front + movement.z * m_up;
		}

	};
}