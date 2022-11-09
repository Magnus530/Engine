/* The game engine series by The Cherno on Youtube, was a helpful source when creating this engine.
https://www.youtube.com/watch?v=JxIZbV_XjAs&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT&index=1 */

#include <Engine.h>

#include "imgui/imgui.h"

class ExampleLayer : public Engine::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{}

	void OnUpdate() override
	{
		if (Engine::Input::IsKeyPressed(E_KEY_TAB))
		{
			E_TRACE("Tab key is pressed (poll).");
		}
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
	}

	void OnEvent(Engine::Event& event) override
	{
		if (event.GetEventType() == Engine::EventType::KeyPressed)
		{
			Engine::KeyPressedEvent& e = (Engine::KeyPressedEvent&)event;
			if (e.GetKeyCode() == E_KEY_TAB)
			{
				E_TRACE("Tab key is pressed (event).");
			}
			E_TRACE("{0}", (char)e.GetKeyCode());
		}
	}
};

class Sandbox : public Engine::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{}
};

Engine::Application* Engine::CreateApplication()
{
	return new Sandbox();
}
