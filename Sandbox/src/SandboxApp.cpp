/* The game engine series by The Cherno on Youtube, was a helpful source when creating this engine.
https://www.youtube.com/watch?v=JxIZbV_XjAs&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT&index=1 */

#include <Engine.h>
#include "Engine/Core/EntryPoint.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Engine/Objects/VisualObject.h"
#include "Engine/AssetLoaders/ObjLoader.h"
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

		m_Texture = Engine::Texture2D::Create("assets/textures/checkerboard.png");
		m_WolfLogoTexture = Engine::Texture2D::Create("assets/textures/wolf.png");

		std::dynamic_pointer_cast<Engine::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<Engine::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
		
		//std::dynamic_pointer_cast<Engine::OpenGLShader>(flatShader)->Bind();
		//std::dynamic_pointer_cast<Engine::OpenGLShader>(flatShader)->UploadUniformInt("u_Color", 0);

		m_ActiveScene = std::make_shared<Engine::Scene>();

		//Create entities here

		m_ObjEntity = Engine::EntityInitializer::GetInstance().EntityInit("Cube", m_ObjVA, m_ActiveScene);
		m_ObjEntity.AddComponent<Engine::RendererComponent>(glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });

		m_SquareEntity = Engine::EntityInitializer::GetInstance().EntityInit(m_SquareVA, m_ActiveScene);
		m_SquareEntity.AddComponent<Engine::RendererComponent>(glm::vec4{ 1.0f, 1.0f, 0.0f, 1.0f });
	}

	void OnUpdate(Engine::Timestep ts) override
	{
		// Update
		m_PCameraController.OnUpdate(ts);

		// Render
		Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Engine::RenderCommand::Clear();

		Engine::Renderer::BeginScene(m_PCameraController.GetCamera());

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		auto flatShader = m_ShaderLibrary.Get("Flat");
		auto textureShader = m_ShaderLibrary.Get("Texture");

		auto flat = std::dynamic_pointer_cast<Engine::OpenGLShader>(flatShader);
		flat->UploadUniformInt("u_ShowCustomColor", 1);
		flat->UploadUniformFloat3("u_Color", glm::vec3(m_SquareEntity.GetComponent<Engine::RendererComponent>().Color));

		//Engine::Renderer::Submit(flatShader, m_SquareVA, glm::mat4(1.0f));

		/* Test posisjonering */	
		static float sin{};
		sin += ts;
		float testmovement = sinf(sin);
		m_Texture->Bind();
		Engine::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0, testmovement,0)));

		//m_WolfLogoTexture->Bind();
		//Engine::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		/* ----- OBJ START ----- */
		auto objTComp = m_ObjEntity.GetComponent<Engine::TransformComponent>();
		Engine::TransformSystem::SetWorldPosition(objTComp, glm::vec3(0, testmovement, 0));
		//glm::vec3 currentPosition(objTComp.m_Position[3]);
		//glm::vec3 tempPos = objTComp.m_Transform[3];
		//glm::vec3 travel = tempPos - currentPosition;
		//glm::mat4 tempP = glm::translate(tempP, travel);
		//glm::mat4 tempM = objTComp.m_Scale * objTComp.m_Rotation * objTComp.m_Position;

		Engine::Renderer::Submit(flatShader, m_ObjVA, objTComp.m_Transform);
		/* ----- OBJ END ----- */

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
			auto& squareColor = m_ObjEntity.GetComponent<Engine::RendererComponent>().Color;
			ImGui::ColorEdit4("Obj Color", glm::value_ptr(squareColor));
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
	//std::shared_ptr<Engine::VertexArray> m_VertexArray;

	std::shared_ptr<Engine::Shader> m_FlatColorShader;
	std::shared_ptr<Engine::VertexArray> m_SquareVA, m_ObjVA;

	std::shared_ptr<Engine::Texture2D> m_Texture, m_WolfLogoTexture;

	std::shared_ptr<Engine::Scene> m_ActiveScene; // Entities
	Engine::Entity m_SquareEntity;
	Engine::Entity m_ObjEntity;

	Engine::PerspectiveCameraController m_PCameraController;
	Engine::OrthographicCameraController m_OCameraController;

	std::shared_ptr<Engine::VisualObject> m_Obj;
	//std::shared_ptr<Engine::EntityInitializer> m_EInit;
};

class Sandbox : public Engine::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		//PushLayer(new PathfindingLayer());
		//PushLayer(new TransformExampleLayer());
	}

	~Sandbox()
	{}
};

Engine::Application* Engine::CreateApplication()
{
	return new Sandbox();
}
