/* The game engine series by The Cherno on Youtube, was a helpful source when creating this engine.
https://www.youtube.com/watch?v=JxIZbV_XjAs&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT&index=1 */

#include <Engine.h>
#include "Engine/Core/EntryPoint.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Engine/Renderer/VertexArray.h"
#include "Engine/AssetInit/ObjLoader.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "Engine/Scene/EntityInitializer.h"
#include "Engine/AssetInit/Material.h"

// Layers
#include "PathfindingLayer.h"
#include "TransformExampleLayer.h"

class ExampleLayer : public Engine::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_OCameraController(1280.0f / 720.0f, true), m_PCameraController(50.0f, 1280.0f / 720.0f, 0.01f, 1000.0f)
	{
		Engine::Renderer::RenderInit();

		m_Texture = Engine::Texture2D::Create("assets/textures/checkerboard.png");
		m_WolfLogoTexture = Engine::Texture2D::Create("assets/textures/wolf.png");
		m_WhiteTexture = Engine::Texture2D::Create("assets/textures/white.png");
		
		m_ActiveScene = std::make_shared<Engine::Scene>();

		//Create entities here

		m_PlaneEntity = Engine::EntityInitializer::GetInstance().EntityInit("Plane", m_PlaneVA, m_ActiveScene);
		m_PlaneEntity.AddComponent<Engine::RendererComponent>(m_PlaneVA);
		m_PlaneEntity.AddComponent<Engine::TextureComponent>(m_WhiteTexture);
		Engine::TransformSystem::SetScale(m_PlaneEntity.GetComponent<Engine::TransformComponent>(), glm::vec3{ 3.0f, 3.0f, 3.0f });
		m_PlaneEntity.AddComponent<Engine::FlatMaterialComponent>(glm::vec4{ 1.f, 1.f, 0.f, 1.f }, m_PlaneEntity.GetComponent<Engine::TransformComponent>().m_Transform);

		m_LightEntity = Engine::EntityInitializer::GetInstance().EntityInit("Sphere", m_SphereVA, m_ActiveScene);
		m_LightEntity.AddComponent<Engine::RendererComponent>(m_SphereVA);
		m_LightEntity.AddComponent<Engine::LightComponent>(float{ 1 });
		Engine::TransformSystem::SetWorldPosition(m_LightEntity.GetComponent<Engine::TransformComponent>(), glm::vec3{ 0.0f, 3.0f, 1.0f });
		Engine::TransformSystem::SetScale(m_LightEntity.GetComponent<Engine::TransformComponent>(), glm::vec3{ 0.5f, 0.5f, 0.5f });

		m_Audio = std::make_shared<Engine::AudioEngine>();
	}

	void OnUpdate(Engine::Timestep ts) override
	{
		m_PCameraController.OnUpdate(ts);
		Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Engine::RenderCommand::Clear();

		Engine::Renderer::BeginScene(m_PCameraController.GetCamera());

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		m_Audio->update(ts);
		//Engine::Renderer::Submit(Engine::ShaderType::Phong, m_PlaneVA, m_PlaneEntity, m_LightEntity, m_PCameraController);
		//Engine::Renderer::Submit(Engine::ShaderType::Texture, m_CubeVA, m_CubeEntity, m_LightEntity, m_PCameraController);
		//Engine::Renderer::Submit(Engine::ShaderType::Flat, m_SphereVA, m_LightEntity, m_LightEntity, m_PCameraController);

		Engine::Renderer::Submit(Engine::ShaderType::Flat, m_PlaneEntity);
		
		Engine::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		if (m_PlaneEntity)
		{
			//ImGui::Separator();
			//auto& tag = m_PlaneEntity.GetComponent<Engine::TagComponent>().Tag;
			//ImGui::Text("%s", tag.c_str());
			//auto& objColor = m_PlaneEntity.GetComponent<Engine::FlatMaterialComponent>().m_Color;
			//ImGui::ColorEdit4("Plane Color", glm::value_ptr(m_PlaneEntity.GetComponent<Engine::FlatMaterialComponent>().m_Color));
			//ImGui::Separator();
			//ImGui::Checkbox("Show Custom Color", &m_PlaneEntity.GetComponent<Engine::RendererComponent>().m_bCustomColor);
			//ImGui::Separator();
		}

		if (m_CubeEntity)
		{
			//ImGui::Separator();
			//auto& tag = m_CubeEntity.GetComponent<Engine::TagComponent>().Tag;
			//ImGui::Text("%s", tag.c_str());
			//auto& objColor = m_CubeEntity.GetComponent<Engine::FlatMaterialComponent>().m_Color;
			//ImGui::ColorEdit4("Cube Color", glm::value_ptr(m_CubeEntity.GetComponent<Engine::FlatMaterialComponent>().m_Color));
			//ImGui::Separator();
			//ImGui::Checkbox("Show Custom Color", &m_CubeEntity.GetComponent<Engine::RendererComponent>().m_bCustomColor);
			//ImGui::Separator();
		}

		ImGui::End();
	}

	void OnEvent(Engine::Event& e) override
	{
		m_PCameraController.OnEvent(e);
	}

private:
	std::shared_ptr<Engine::Shader> m_Shader;

	std::shared_ptr<Engine::AudioEngine> m_Audio;
	std::shared_ptr<Engine::Shader> m_FlatColorShader;
	std::shared_ptr<Engine::VertexArray> m_PlaneVA, m_CubeVA, m_SphereVA;

	std::shared_ptr<Engine::Texture2D> m_Texture, m_WolfLogoTexture, m_WhiteTexture;

	std::shared_ptr<Engine::Material> FlatMaterial;

	Engine::PerspectiveCameraController m_PCameraController;
	Engine::OrthographicCameraController m_OCameraController;


	std::shared_ptr<Engine::Scene> m_ActiveScene;

	// Entities
	Engine::Entity m_CubeEntity;
	Engine::Entity m_PlaneEntity;
	Engine::Entity m_LightEntity;
};

class Sandbox : public Engine::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushLayer(new PathfindingLayer());
		PushLayer(new TransformExampleLayer());
		
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
