#pragma once

#include "Core.h"

#include "Window.h"
#include "Engine/Core/LayerStack.h"
#include "Engine/Events/Event.h"
#include "Engine/Events/ApplicationEvent.h"

#include "Engine/Core/Timestep.h"

#include "Engine/ImGui/ImGuiLayer.h"

namespace Engine
{
	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);


	public:	// For Editor Gui Layer
		uint32_t GetLayerCount() const { return m_LayerCount; }
		std::vector<std::string> GetLayerNames() const;
		Layer* GetLayerAtIndex(uint32_t index);
		void SetCurrentLayer(uint32_t index);
	protected:
		void SetGuiLayerNames();
	private:
		uint32_t m_LayerCount{};
		Layer* m_CurrentLayer{ nullptr };
	private:
		std::unique_ptr<Window> m_Window;
		//ImGuiLayer* m_ImGuiLayer = nullptr;
		class EditorGuiLayer* m_EditorGuiLayer = nullptr;	// Forandret typen Gui layer
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;

		static Application* s_Instance;
	};


	// To be defined in client.
	Application* CreateApplication();


	class EditorGuiLayer : public ImGuiLayer
	{
	public:
		void OnImGuiRender() override;

	public:
		void SetApplication(Application* app) { m_App = app; }
		void SetLayerNames(std::vector<std::string> layernames) { m_LayerNames = layernames; }
	private:
		std::vector<std::string> m_LayerNames;
		Application* m_App{ nullptr };
	};

	//class RayCast
	//{
	//public:
	//	static void OnWindowResize(unsigned int width, unsigned int height);

	//public:	// Raycast functions
	//	static void FromScreenPosition(glm::vec3& ray, glm::vec2 screenPosition, glm::mat4 projection, glm::mat4 view);

	//private:
	//	struct WindowSize
	//	{
	//		unsigned int X = 1280;
	//		unsigned int Y = 720;
	//	};
	//	static WindowSize* m_WindowSize;
	//};
}
