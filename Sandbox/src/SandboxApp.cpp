/* The game engine series by The Cherno on Youtube, was a helpful source when creating this engine.
https://www.youtube.com/watch?v=JxIZbV_XjAs&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT&index=1 */

#include "Engine.h"
#include "Engine/Core/EntryPoint.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Engine/AssetInit/ObjLoader.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "Platform/OpenGL/OpenGLCubemap.h"

#include <unordered_map>

// Layers
#include "PathfindingLayer.h"
#include "TransformExampleLayer.h"

class MainLayer : public Engine::Layer
{
public:
	MainLayer()
		: Layer("Main"), m_OCameraController(1280.0f / 720.0f, true), m_PCameraController(50.0f, 1280.0f / 720.0f, 0.01f, 1000.0f)
	{
		m_ActiveScene = std::make_shared<Engine::Scene>();
		m_Audio = std::make_shared<Engine::AudioEngine>();
		Engine::Renderer::RenderInit();
		Engine::TextureList::CreateTextures(m_ActiveScene);

		//Create entities here

		m_LightEntity = Engine::EntityInitializer::GetInstance().EntityInit(Engine::ShaderType::Flat, "Sphere", m_SphereVA, m_ActiveScene, glm::vec3{ 1.0f, 1.0f, 0.0f });
		m_LightEntity.AddComponent<Engine::LightComponent>(float{ 0.2f }, float { 2.f });
		Engine::TransformSystem::SetWorldPosition(m_LightEntity.GetComponent<Engine::TransformComponent>(), glm::vec3{ 0.0f, 3.0f, 1.0f });
		Engine::TransformSystem::SetScale(m_LightEntity.GetComponent<Engine::TransformComponent>(), glm::vec3{ 0.5f, 0.5f, 0.5f });

		m_SkyboxEntity = Engine::EntityInitializer::GetInstance().EntityInit("Cube", m_SkyboxVA, m_ActiveScene, *m_ActiveScene->m_Skyboxes.find("Sky"));

		m_PlaneEntity = Engine::EntityInitializer::GetInstance().EntityInit(Engine::ShaderType::Phong, "Plane", m_PlaneVA, m_ActiveScene, glm::vec3{ 0.0f, 0.0f, 1.0f });
		Engine::TransformSystem::SetScale(m_PlaneEntity.GetComponent<Engine::TransformComponent>(), glm::vec3{ 3.0f, 3.0f, 3.0f });
	}

	void OnUpdate(Engine::Timestep ts) override
	{
		m_PCameraController.OnUpdate(ts);
		m_Audio->update(ts);
		Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Engine::RenderCommand::Clear();

		Engine::Renderer::BeginScene(m_PCameraController.GetCamera());

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));



		for (auto& it = m_ActiveScene->m_Entities.begin(); it != m_ActiveScene->m_Entities.end(); it++)
		{
			Engine::Renderer::Submit(*(it)->second);

			if ((it)->second->HasComponent<Engine::PhongMaterialComponent>())
			{
				Engine::LightSystem::UpdateLight((it)->second->GetComponent<Engine::PhongMaterialComponent>(), m_LightEntity, m_PCameraController);
			}
		}

		Engine::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		Engine::ImGuiSystem::GuiEntitySettings(m_ActiveScene);
	}

	void OnEvent(Engine::Event& e) override
	{
		m_PCameraController.OnEvent(e);
	}

private:
	std::shared_ptr<Engine::Scene> m_ActiveScene;
	std::shared_ptr<Engine::AudioEngine> m_Audio;

	Engine::PerspectiveCameraController m_PCameraController;
	Engine::OrthographicCameraController m_OCameraController;

	std::shared_ptr<Engine::VertexArray> m_PlaneVA, m_CubeVA, m_SphereVA, m_SkyboxVA;

	// Entities
	Engine::Entity m_CubeEntity;
	Engine::Entity m_PlaneEntity;
	Engine::Entity m_LightEntity;
	Engine::Entity m_SkyboxEntity;
};

class Sandbox : public Engine::Application
{
public:
	Sandbox()
	{
		PushLayer(new MainLayer());
		//PushLayer(new PathfindingLayer());
		//PushLayer(new TransformExampleLayer());
		
		SetCurrentLayer(0);
		SetGuiLayerNames();
	}

	~Sandbox()
	{}

private:
	std::shared_ptr<Engine::Layer> m_CurrentLayer;
};

Engine::Application* Engine::CreateApplication()
{
	return new Sandbox();
}
