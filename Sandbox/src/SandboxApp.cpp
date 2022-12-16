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

// Layers
#include "PathfindingLayer.h"
#include "TransformExampleLayer.h"

class ExampleLayer : public Engine::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_OCameraController(1280.0f / 720.0f, true), m_PCameraController(50.0f, 1280.0f / 720.0f, 0.01f, 1000.0f)
	{
		auto flatShader = m_ShaderLibrary.Load("assets/shaders/Flat.glsl");
		auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");
		auto phongShader = m_ShaderLibrary.Load("assets/shaders/Phong.glsl");

		m_Texture = Engine::Texture2D::Create("assets/textures/checkerboard.png");
		m_WolfLogoTexture = Engine::Texture2D::Create("assets/textures/wolf.png");

		std::dynamic_pointer_cast<Engine::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<Engine::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
		
		m_ActiveScene = std::make_shared<Engine::Scene>();

		//Create entities here

		m_ObjEntity = Engine::EntityInitializer::GetInstance().EntityInit("Plane", m_PlaneVA, m_ActiveScene);
		m_ObjEntity.AddComponent<Engine::RendererComponent>(glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });
		Engine::TransformSystem::SetWorldPosition(m_ObjEntity.GetComponent<Engine::TransformComponent>(), glm::vec3{ 2.0f, 1.0f, 1.0f });


		m_CubeEntity = Engine::EntityInitializer::GetInstance().EntityInit("Cube", m_CubeVA, m_ActiveScene);
		m_CubeEntity.AddComponent<Engine::RendererComponent>(glm::vec4{ 0.0f, 0.0f, 1.0f, 1.0f });
		m_CubeEntity.GetComponent<Engine::RendererComponent>().m_Tex = m_Texture;

		//m_PrimitiveCubeEntity = Engine::EntityInitializer::GetInstance().EntityInit(2, m_PrimitiveVA, m_ActiveScene);
		//m_PrimitiveCubeEntity.AddComponent<Engine::RendererComponent>(glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });

		m_Audio = std::make_shared<Engine::AudioEngine>();
	}

	void OnUpdate(Engine::Timestep ts) override
	{
		m_PCameraController.OnUpdate(ts);
		Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Engine::RenderCommand::Clear();

		Engine::Renderer::BeginScene(m_PCameraController.GetCamera());

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		auto flatShader = m_ShaderLibrary.Get("Flat");
		auto textureShader = m_ShaderLibrary.Get("Texture");
		auto phongShader = m_ShaderLibrary.Get("Phong");

		m_Audio->update(ts);
		/* Test posisjonering */	
		//static float sin{};
		//sin += ts;
		//float testmovement = sinf(sin);
		//	glm::vec3(1.5f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0, testmovement, 0)));

		Engine::Renderer::Submit(Engine::ShaderType::Flat, flatShader, m_PlaneVA, m_ObjEntity);
		Engine::Renderer::Submit(Engine::ShaderType::Texture, textureShader, m_CubeVA, m_CubeEntity);
		//Engine::Renderer::Submit(Engine::ShaderType::Texture, textureShader, m_PrimitiveVA, m_PrimitiveCubeEntity);
		
		Engine::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		if (m_ObjEntity)
		{
			ImGui::Separator();
			auto& tag = m_ObjEntity.GetComponent<Engine::TagComponent>().Tag;
			ImGui::Text("%s", tag.c_str());
			auto& objColor = m_ObjEntity.GetComponent<Engine::RendererComponent>().m_Color;
			ImGui::ColorEdit4("Obj Color", glm::value_ptr(m_ObjEntity.GetComponent<Engine::RendererComponent>().m_Color));
			ImGui::Separator();
			ImGui::Checkbox("Show Custom Color", &m_ObjEntity.GetComponent<Engine::RendererComponent>().m_bCustomColor);
			ImGui::Separator();
		}

		if (m_CubeEntity)
		{
			ImGui::Separator();
			auto& tag = m_CubeEntity.GetComponent<Engine::TagComponent>().Tag;
			ImGui::Text("%s", tag.c_str());
			auto& objColor = m_CubeEntity.GetComponent<Engine::RendererComponent>().m_Color;
			ImGui::ColorEdit4("Plane Color", glm::value_ptr(m_CubeEntity.GetComponent<Engine::RendererComponent>().m_Color));
			ImGui::Separator();
			ImGui::Checkbox("Show Custom Color", &m_CubeEntity.GetComponent<Engine::RendererComponent>().m_bCustomColor);
			ImGui::Separator();
		}

		ImGui::End();
	}

	void OnEvent(Engine::Event& e) override
	{
		m_PCameraController.OnEvent(e);
	}

private:
	Engine::ShaderLibrary m_ShaderLibrary;
	std::shared_ptr<Engine::Shader> m_Shader;

	std::shared_ptr<Engine::AudioEngine> m_Audio;
	std::shared_ptr<Engine::Shader> m_FlatColorShader;
	std::shared_ptr<Engine::VertexArray> m_PlaneVA, m_CubeVA;

	std::shared_ptr<Engine::Texture2D> m_Texture, m_WolfLogoTexture;

	Engine::PerspectiveCameraController m_PCameraController;
	Engine::OrthographicCameraController m_OCameraController;

	std::shared_ptr<Engine::Scene> m_ActiveScene;

	// Entities
	Engine::Entity m_CubeEntity;
	Engine::Entity m_ObjEntity;
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
