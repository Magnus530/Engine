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


// Layers
#include "PathfindingLayer.h"


class ExampleLayer : public Engine::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_OCameraController(1280.0f / 720.0f, true), m_PCameraController(50.0f, 1280.0f / 720.0f, 0.01f, 1000.0f)
	{
		//m_VertexArray.reset(Engine::VertexArray::Create());

		//float squareVertices[5 * 4] =
		//{
		////    x      y	     z?			uv
		//	-0.5f, -0.5f,   0.0f,	0.0f, 0.0f,	//	Bottom	- Left 
		//	 0.5f, -0.5f,   0.0f,	1.0f, 0.0f, //	Bottom	- Right
		//	 0.5f,  0.5f,   0.0f,	1.0f, 1.0f, //	Top		- Right
		//	-0.5f,  0.5f,   0.0f,	0.0f, 1.0f	//	Top		- Left
		//};
		// 
		// m_SquareVA = Engine::VertexArray::Create();	// Ref<OpenGLVertexArray>
		m_SquareVA.reset(Engine::VertexArray::Create()); // OpenGLVertexArray*
		std::vector<float> squareVertices =
		{
			//    x      y	     z?		 normal		    uv
				-0.5f, -0.5f,   0.0f,	 0,0,0,		0.0f, 0.0f,	//	Bottom	- Left 
				 0.5f, -0.5f,   0.0f,	 0,0,0,		1.0f, 0.0f, //	Bottom	- Right
				 0.5f,  0.5f,   0.0f,	 0,0,0,		1.0f, 1.0f, //	Top		- Right
				-0.5f,  0.5f,   0.0f,	 0,0,0,		0.0f, 1.0f	//	Top		- Left
		};
		
		std::shared_ptr<Engine::VertexBuffer> SquareVB;
		SquareVB.reset(Engine::VertexBuffer::Create(squareVertices.data(), squareVertices.size()*sizeof(float))); // OpenGLVertexBuffer*	// for en vector av floats
		SquareVB->SetLayout
		({
			{ Engine::ShaderDataType::Float3, "a_Position" },
			{ Engine::ShaderDataType::Float3, "a_Normal" },
			{ Engine::ShaderDataType::Float2, "a_TexCoord" }
			});
		m_SquareVA->AddVertexBuffer(SquareVB);
		
		std::vector<uint32_t> squareIndices = { 0, 1, 2, 2, 3, 0 };
		
		std::shared_ptr<Engine::IndexBuffer> SquareIB;
		SquareIB.reset(Engine::IndexBuffer::Create(squareIndices)); // OpenGLIndexBuffer*
		m_SquareVA->SetIndexBuffer(SquareIB);

		auto flatShader = m_ShaderLibrary.Load("assets/shaders/Flat.glsl");
		auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = Engine::Texture2D::Create("assets/textures/checkerboard.png");
		m_WolfLogoTexture = Engine::Texture2D::Create("assets/textures/wolf.png");

		std::dynamic_pointer_cast<Engine::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<Engine::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
		
		std::dynamic_pointer_cast<Engine::OpenGLShader>(flatShader)->Bind();
		std::dynamic_pointer_cast<Engine::OpenGLShader>(flatShader)->UploadUniformInt("u_Color", 0);

		m_ActiveScene = std::make_shared<Engine::Scene>();

		// Create entities here
		auto square = m_ActiveScene->CreateEntity("Square");
		square.AddComponent<Engine::RendererComponent>(glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});

		m_SquareEntity = square;
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

		auto textureShader = m_ShaderLibrary.Get("Texture");
		auto flatShader = m_ShaderLibrary.Get("Flat");


		auto& squareColor = m_SquareEntity.GetComponent<Engine::RendererComponent>().Color;
		std::dynamic_pointer_cast<Engine::OpenGLShader>(flatShader)->UploadUniformFloat3("u_Color", squareColor);

		Engine::Renderer::Submit(flatShader, m_SquareVA, glm::mat4(1.0f));

		m_Texture->Bind();
		/* Test posisjonering */	
		static float sin{};
		sin += ts;
		float testmovement = sinf(sin);
		Engine::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0, testmovement,0)));
		//Engine::Renderer::Submit(textureShader, m_VA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		m_WolfLogoTexture->Bind();
		Engine::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		Engine::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		if (m_SquareEntity)
		{
			ImGui::Separator();
			auto& tag = m_SquareEntity.GetComponent<Engine::TagComponent>().Tag;
			ImGui::Text("%s", tag.c_str());
			auto& squareColor = m_SquareEntity.GetComponent<Engine::RendererComponent>().Color;
			ImGui::ColorEdit4("Square Color", glm::value_ptr(squareColor));
			m_SquareEntity.GetComponent<Engine::RendererComponent>().Color = squareColor;
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
	std::shared_ptr<Engine::VertexArray> m_SquareVA;

	std::shared_ptr<Engine::Texture2D> m_Texture, m_WolfLogoTexture;

	std::shared_ptr<Engine::Scene> m_ActiveScene; // Entities
	Engine::Entity m_SquareEntity;

	Engine::PerspectiveCameraController m_PCameraController;
	Engine::OrthographicCameraController m_OCameraController;
	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
};

class Sandbox : public Engine::Application
{
public:
	Sandbox()
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new PathfindingLayer());
	}

	~Sandbox()
	{}
};

Engine::Application* Engine::CreateApplication()
{
	return new Sandbox();
}
