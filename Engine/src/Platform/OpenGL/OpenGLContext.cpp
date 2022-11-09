#include "epch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <GL/GL.h>

namespace Engine
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		E_CORE_ASSERT(windowHandle, "The Window handle is null.");

	}
	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		E_CORE_ASSERT(status, "There is an issue when initializing GLAD.");

		E_CORE_INFO("OpenGL Info:");
		E_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		E_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		E_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));
	}
	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}