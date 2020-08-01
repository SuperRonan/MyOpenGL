#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "MouseHandler.h"

namespace lib
{
	class Window
	{
	protected:

		using Vector2i = Vector2<int>;

		GLFWmonitor* m_monitor;
		GLFWwindow* m_window;

	public:

		Window(int width, int height, const char* name, bool center = true);

		Window(Window const&) = delete;

		Window(Window&&) = default;

		~Window();

		bool isOk()const;

		int width()const;

		int height()const;

		Vector2i size()const;

		GLFWwindow* get();

		void makeCurrent();

		bool shouldClose()const;

		void swapBuffers();

	};
}