#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

namespace lib
{
	class MouseHandler
	{
	public:

		enum Mode {Position, Direction};
	
	protected:


		using Vector2d = glm::vec < 2, double>;
		using Vector2f = glm::vec < 2, float>;


		Vector2d m_prev_pos, m_current_pos;
		Vector2d m_delta;

		GLFWwindow* m_window;
		
		

		// in degrees
		double m_pitch, m_yaw;

		void updatePhiTheta();

		Mode m_mode;

	public:

		float fov;
		
		double sensibility;

		MouseHandler(GLFWwindow * window, Mode mode = Mode::Direction, double sensibility=1.0);

		MouseHandler(MouseHandler const&) = delete;
		MouseHandler(MouseHandler &&) = delete;

		void update(double dt);

		template <class Float>
		Vector2d currentPosition()const
		{
			glm::vec<2, Float> res;
			res.x = m_current_pos.x;
			res.y = m_current_pos.y;
			return res;
		}

		template <class Float>
		glm::vec<3, Float> direction()const
		{
			glm::vec<3, Float> res;
			double r_yaw = glm::radians(m_yaw), r_pitch = -glm::radians(m_pitch);
			res.x = std::cos(r_yaw) * std::cos(r_pitch);
			res.y = std::sin(r_pitch);
			res.z = std::sin(r_yaw) * std::cos(r_pitch);
			return res;
		}

		bool inWindow()const;


		void setMode(Mode mode);

		template <class Out>
		Out& print(Out& out)const
		{
			out << "pos: (" << m_current_pos.x << ", "<<m_current_pos.y<<"), " <<
				"delta: (" << m_delta.x <<", "<<m_delta.y << "), pitch: "<<m_pitch<<", yaw: "<<m_yaw<<"\n";
			return out;
		}
	};
}