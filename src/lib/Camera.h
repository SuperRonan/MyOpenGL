#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include "Math.h"
#include "Component.h"

namespace lib
{
	template <class Float>
	class Camera : public Component
	{
	public:

		enum class Type {Perspective, Orthographic};

	protected:

		using Vector3 = Vector3<Float>;
		using Matrix4 = Matrix4<Float>;

		Type m_type;

		Vector3 m_position;
		Vector3 m_front;
		Vector3 m_up;
		Vector3 m_right;

		Float m_fov = 90;
		Float m_aspect = 16.0 / 9.0;
		Vector2<Float> m_clip = { 0.01, 1000 };

		__forceinline void updateRight()
		{
			m_right = glm::normalize(glm::cross(m_front, m_up));
		}

	public:

		Camera(Vector3 const& position = { 0, 0, 0 }, Vector3 const& front = { 0, 0, -1 }) :
			m_type(Type::Perspective),
			m_position(position),
			m_front(glm::normalize(front)),
			m_up(Float(0), Float(1), Float(0)),
			m_right(glm::cross(m_front, m_up))
		{}
		
		Camera(Camera const& other) = default;

		virtual ~Camera() = default;

		Matrix4 getMatrixV()const
		{
			return glm::lookAt(m_position, m_position + m_front, m_up);
		}

		Matrix4 getMatrixP()const
		{
			return m_type == Type::Perspective ? glm::perspective<Float>(glm::radians(m_fov), m_aspect, m_clip[0], m_clip[1]) :
				glm::ortho<Float>(-1, 1, -1, 1);
		}

		Vector3 getPosition()const
		{
			return m_position;
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

		void setPosition(Vector3 const& pos)
		{
			m_position = pos;
		}

		void move(Vector3 const& movement)
		{
			m_position += movement.x * m_right + movement.y * m_front + movement.z * m_up;
		}

		void setType(Type type)
		{
			m_type = type;
		}
	};
}