#include "epch.h"
#include "Application.h"
#include "Input.h"
#include "imgui.h"

#include "Engine/Renderer/Renderer.h"
#include "Engine/RayCast/RayCast.h"
#include "Engine/Core/ImGuiSystem.h"

#include <GLFW/glfw3.h>

namespace Engine
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		E_CORE_ASSERT(!s_Instance, "Application already exists.");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		Renderer::Init();

		m_EditorGuiLayer = new EditorGuiLayer();
		PushOverlay(m_EditorGuiLayer);
		m_EditorGuiLayer->SetApplication(this);

			
		Renderer::OnWindowResize(m_Window->GetWidth(), m_Window->GetHeight());
		RayCast::OnWindowResize(m_Window->GetWidth(), m_Window->GetHeight());
	}

	Application::~Application()
	{}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerCount++;	// Only increment on pushing non-overlay
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

		// EditorLayer gets events first, then the CurrentLayer has a chance to recieve that event
		m_EditorGuiLayer->OnEvent(e);
		if (e.Handled)
			return;
		m_CurrentLayer->OnEvent(e);

		// Removed Events going through the whole Layerstack, will now only go through the m_CurrentLayer - Adrian
		//for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		//{
		//	(*--it)->OnEvent(e);
		//	if (e.Handled)
		//	{
		//		break;
		//	}
		//}
	}

	void Application::Run()
	{
		while (m_Running)
		{
			float time = (float)glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;


			if (!m_Minimized)
			{
				//for (Layer* layer : m_LayerStack)
				//{
				//	layer->OnUpdate(timestep);
				//}
				m_CurrentLayer->OnUpdate(timestep);	// Only update current layer
			}

			m_EditorGuiLayer->Begin();
			//for (Layer* layer : m_LayerStack)
			//{
			//	layer->OnImGuiRender();
			//}
			m_EditorGuiLayer->OnImGuiRender();
			m_CurrentLayer->OnImGuiRender();	// Only update current layer
			m_EditorGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
		RayCast::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

	void Application::SetGuiLayerNames()
	{
		m_EditorGuiLayer->SetLayerNames(GetLayerNames());
	}

	std::vector<std::string> Application::GetLayerNames() const
	{
		return m_LayerStack.GetLayerNames();
	}

	Layer* Application::GetLayerAtIndex(uint32_t index)
	{
		return m_LayerStack.GetLayerAtIndex(index);
	}

	void Application::SetCurrentLayer(uint32_t index)
	{
		m_CurrentLayer = GetLayerAtIndex(index);
	}

	/************* Editor Gui Layer ***************/
	void EditorGuiLayer::OnImGuiRender()
	{
		std::shared_ptr<ImGuiSystem> gui = std::make_shared<ImGuiSystem>();
		gui->LayerSelection(m_App, m_LayerNames);
	}

}