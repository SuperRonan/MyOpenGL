#include "Window.h"

#include <iostream>


namespace lib
{
	
	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	Window::Window(int width, int height, const char* name, bool center) :
		m_monitor(glfwGetPrimaryMonitor()),
		m_window(glfwCreateWindow(width, height, name, NULL, NULL))
	{
		if (isOk())
		{
			glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
			//glViewport(0, 0, width, height);
			if (center)
			{
				const GLFWvidmode* mode = glfwGetVideoMode(m_monitor);
				glfwSetWindowPos(m_window, (mode->width - width) / 2, (mode->height - height) / 2);
			}
		}
		else
		{
			std::cerr << "Could not create the window!" << std::endl;
		}
	}

	Window::~Window()
	{
		if(isOk())
			glfwDestroyWindow(m_window);
		m_window = NULL;
	}

	bool Window::isOk()const
	{
		return m_window != NULL;
	}

	int Window::width()const
	{
		return size()[0];
	}

	int Window::height()const
	{
		return size()[1];
	}

	Window::Vector2i Window::size()const
	{
		Vector2i res;
		glfwGetWindowSize(m_window, &res[0], &res[1]);
		return res;
	}

	GLFWwindow* Window::get()
	{
		return m_window;
	}

	void Window::makeCurrent()
	{
		glfwMakeContextCurrent(m_window);
	}

	bool Window::shouldClose()const
	{
		return glfwWindowShouldClose(m_window);
	}

	void Window::swapBuffers()
	{
		glfwSwapBuffers(m_window);
	}
}