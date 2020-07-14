#include "MouseHandler.h"
#include <cmath>

namespace lib
{

	void MouseHandler::updatePhiTheta()
	{
		const double base_sensitivity = 0.1;
		m_pitch += m_delta.y * sensibility * base_sensitivity;
		m_yaw += m_delta.x * sensibility * base_sensitivity;
		const double max_pitch = 89.0;
		if (m_pitch < -max_pitch)
			m_pitch = -max_pitch;
		else if (m_pitch > max_pitch)
			m_pitch = max_pitch;
		m_yaw = std::fmod(m_yaw, 360.0);
	}

	MouseHandler::MouseHandler(GLFWwindow* window, MouseHandler::Mode mode, double sensibility) :
		m_prev_pos(0.0, 0.0),
		m_current_pos(0.0, 0.0),
		m_delta(0.0, 0.0),
		m_pitch(0.0),
		m_yaw(180.0),
		m_window(window),
		m_mode(mode),
		fov(90),
		sensibility(sensibility)
	{
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		glfwSetCursorPos(window, double(w)/2.0, double(h)/2.0);
		setMode(m_mode);
	}

	void MouseHandler::update(double dt)
	{
		m_prev_pos = m_current_pos;
		glfwGetCursorPos(m_window, &m_current_pos.x, &m_current_pos.y);
		m_delta = m_current_pos - m_prev_pos;
		if (m_mode == Mode::Direction)
		{
			m_prev_pos = { 0.0, 0.0 };
			glfwSetCursorPos(m_window, 0.0, 0.0);
			updatePhiTheta();
		}
	}



	bool MouseHandler::inWindow()const
	{
		
	}

	void MouseHandler::setMode(MouseHandler::Mode mode)
	{
		if (mode != m_mode)
		{
			m_mode = mode;
			if (m_mode == Mode::Direction)
			{
				glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			else if(m_mode == Mode::Position)
			{
				glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
		}
	}

	
}