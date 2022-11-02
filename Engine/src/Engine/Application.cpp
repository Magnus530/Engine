#include "epch.h"
#include "Application.h"

#include "Engine/Events/Event.h"
#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Log.h"

namespace Engine
{
	Application::Application()
	{}

	Application::~Application()
	{}

	void Application::Run()
	{
		WindowResizeEvent e(1280, 720);
		E_TRACE(e);

		while (true);
	}
}

