#include "MouseHandler.h"
#include <cmath>
#include <cstring>

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
		m_mode(mode),
		m_prev_pos(0.0, 0.0),
		m_current_pos(0.0, 0.0),
		m_delta(0.0, 0.0),
		m_pitch(0.0),
		m_yaw(180.0),
		m_window(window),
		fov(90),
		sensibility(sensibility)
	{
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		glfwSetCursorPos(window, double(w)/2.0, double(h)/2.0);
		setMode(m_mode);
		for (int i = 0; i < 5; ++i)
		{
			m_current_buttons[i] = GLFW_RELEASE;
			m_prev_buttons[i] = GLFW_RELEASE;
		}
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
		std::memcpy(m_prev_buttons, m_current_buttons, sizeof(m_current_buttons));
		for (int i = 0; i < 5; ++i)
		{
			m_current_buttons[i] = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_1 + i);
		}
	}



	bool MouseHandler::inWindow()const
	{
		return false;
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


	bool MouseHandler::isButtonCurrentlyPressed(int id)const
	{
		assert(id < 5 && id >= 0);
		return m_current_buttons[id];
	}
	
	bool MouseHandler::isButtonJustPressed(int id)const
	{
		assert(id < 5 && id >= 0);
		return m_current_buttons[id] && !m_prev_buttons[id];
	}

	bool MouseHandler::isButtonJustReleased(int id)const
	{
		assert(id < 5 && id >= 0);
		return !m_current_buttons[id] && m_prev_buttons[id];
	}
}