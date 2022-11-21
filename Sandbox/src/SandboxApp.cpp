/* The game engine series by The Cherno on Youtube, was a helpful source when creating this engine.
https://www.youtube.com/watch?v=JxIZbV_XjAs&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT&index=1 */

#include <Engine.h>

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Engine/Renderer/VisualObject/VisualObject.h"
#include "Engine/AssetLoaders/ObjLoader.h"



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
			//    x      y	     z?			uv
				-0.5f, -0.5f,   0.0f,	0.0f, 0.0f,	//	Bottom	- Left 
				 0.5f, -0.5f,   0.0f,	1.0f, 0.0f, //	Bottom	- Right
				 0.5f,  0.5f,   0.0f,	1.0f, 1.0f, //	Top		- Right
				-0.5f,  0.5f,   0.0f,	0.0f, 1.0f	//	Top		- Left
		};
		
		std::shared_ptr<Engine::VertexBuffer> SquareVB;
		SquareVB.reset(Engine::VertexBuffer::Create(squareVertices.data(), squareVertices.size()*sizeof(float))); // OpenGLVertexBuffer*	// for en vector av floats
		// SquareVB = Engine::VertexBuffer::Create(squareVertices.data(), squareVertices.size()*sizeof(float)); //Ref<OpenGLVertexBuffer>	// for en vector av floats
		SquareVB->SetLayout
		({
			{ Engine::ShaderDataType::Float3, "a_Position" },
			{ Engine::ShaderDataType::Float2, "a_TexCoord" }
			});
		m_SquareVA->AddVertexBuffer(SquareVB);
		
		std::vector<uint32_t> squareIndices = { 0, 1, 2, 2, 3, 0 };
		
		std::shared_ptr<Engine::IndexBuffer> SquareIB;
		SquareIB.reset(Engine::IndexBuffer::Create(squareIndices.data(), squareIndices.size())); OpenGLIndexBuffer*
		// SquareIB = Engine::IndexBuffer::Create(squareIndices.data(), squareIndices.size());	// Ref<OpenGLIndexBuffer>
		m_SquareVA->SetIndexBuffer(SquareIB);




		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ProjectionView;
			uniform mat4 u_ViewMatrix;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ProjectionView * u_ViewMatrix * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

		m_Shader = Engine::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);

		std::string flatColorShaderVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ProjectionView;
			uniform mat4 u_ViewMatrix;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ProjectionView * u_ViewMatrix * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string flatColorShaderFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			uniform vec3 u_Color;


			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";

		m_FlatColorShader = Engine::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);

		auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = Engine::Texture2D::Create("assets/textures/checkerboard.png");
		m_WolfLogoTexture = Engine::Texture2D::Create("assets/textures/wolf.png");

		std::dynamic_pointer_cast<Engine::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<Engine::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);

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

		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_FlatColorShader)->Bind();
		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

		for (int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Engine::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}

		auto textureShader = m_ShaderLibrary.Get("Texture");

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


	std::shared_ptr<Engine::VertexArray> m_VA;
	std::shared_ptr<Engine::VisualObject> m_Obj;

	std::shared_ptr<Engine::Texture2D> m_Texture, m_WolfLogoTexture;

	std::shared_ptr<Engine::Scene> m_ActiveScene; // Entities
	Engine::Entity m_SquareEntity;

	Engine::PerspectiveCameraController m_PCameraController;
	Engine::OrthographicCameraController m_OCameraController;
	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };


};

class New3DLayer : public Engine::Layer
{
public:
	New3DLayer()
		: Layer("New3DLayer"), m_OCameraController(1280.0f / 720.0f, true), m_PCameraController(50.0f, 1280.0f / 720.0f, 0.01f, 1000.0f)
	{
		std::string vertexSrc = R"(
			#version 410 core

			layout(location = 0) in vec3 PositionIn;
			layout(location = 1) in vec4 colorIn;
			layout(location = 2) in vec2 uvIn;

			out vec4 color;
			
			uniform mat4 u_ProjectionView;
			uniform mat4 u_ViewMatrix;
			uniform mat4 u_Transform;

			void main()
			{
				color = colorIn;
				gl_Position = u_ProjectionView * u_ViewMatrix * u_Transform * vec4(PositionIn, 1);
			}
		)";

		std::string fragmentSrc = R"(
			#version 410 core

			in vec4 color;
			out vec4 fragmentColor;

			void main()
			{
				fragmentColor = color;
			}
		)";

		m_Shader = Engine::Shader::Create("FlatColor", vertexSrc, fragmentSrc);
		m_vShader = std::make_shared<Engine::vShader>(vertexSrc, fragmentSrc);


		/* Loading obj */
		std::vector<Engine::Vertex> vertices;
		std::vector<uint32_t> indices;
		Engine::ObjLoader::Get()->ReadFile("BeveledCube", vertices, indices);


		m_Obj = std::make_unique<Engine::VisualObject>(vertices, indices);
		m_Obj->Init(m_VA);
		m_Obj->Init();
	}

	void OnUpdate(Engine::Timestep ts) override
	{
		// Update 
		m_PCameraController.OnUpdate(ts);

		// Begin Render Scene
		Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Engine::RenderCommand::Clear();

		Engine::Renderer::BeginScene(m_PCameraController.GetCamera());

		// Render Objects
		glm::mat4 projectionmatrix = m_PCameraController.GetCamera().GetProjectionMatrix();
		glm::mat4 viewmatrix = m_PCameraController.GetCamera().GetViewMatrix();

		/* New method of rendering object */
		Engine::Renderer::Submit(m_Shader, m_VA, m_Obj->GetMatrix());
		//Engine::Renderer::Submit(m_Shader, m_Obj); 

		/* Old method, similar to 3Dprog */
		glm::mat4 pos = glm::translate(m_Obj->GetMatrix(), glm::vec3(5, 0, 0));
		m_vShader->Use();
		m_vShader->SetUniformMatrix("u_Transform", pos);
		m_vShader->SetUniformMatrix("u_ProjectionView", projectionmatrix);
		m_vShader->SetUniformMatrix("u_ViewMatrix", viewmatrix);
		m_Obj->Draw();


		// End Render Scene
		Engine::Renderer::EndScene();
	}

private:
	Engine::ShaderLibrary m_ShaderLibrary;
	std::shared_ptr<Engine::Shader> m_Shader;
	std::shared_ptr<Engine::vShader> m_vShader;
	

	std::shared_ptr<Engine::VertexArray> m_VA;
	std::unique_ptr<Engine::VisualObject> m_Obj;

	Engine::PerspectiveCameraController m_PCameraController;
	Engine::OrthographicCameraController m_OCameraController;
};

class PathfinderLayer : public Engine::Layer
{
public:
	PathfinderLayer()
	{
		Engine::Pathfinder::SpawnGrid();
	}
	~PathfinderLayer(){}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Pathfinder");
		using namespace Engine;	// Tmp, for readability

		static std::string currentStart{ NULL };
		static std::string currentEnd{ NULL };
		static std::shared_ptr<PNode> start;
		static std::shared_ptr<PNode> end;
		if (ImGui::BeginCombo("StartNode", currentStart.c_str())) {
			for (const auto& it : Pathfinder::mNodes) 
			{
				if (ImGui::Selectable(it->m_name.c_str())) {
					currentStart = it->m_name;
					start = it;
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::BeginCombo("EndNode", currentEnd.c_str())) {
			for (const auto& it : Pathfinder::mNodes)
			{
				if (ImGui::Selectable(it->m_name.c_str())) {
					currentEnd = it->m_name;
					end = it;
				}
			}
			ImGui::EndCombo();
		}

		if (ImGui::Button("Find path")) {
			if (start.get() && end.get()) {
				E_TRACE("Finding path between from {0} to {1}", start->m_name, end->m_name);
				Pathfinder::FindPath(start.get(), end.get());
			}
		}
		ImGui::End();
	}
};

class Sandbox : public Engine::Application
{
public:
	Sandbox()
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new New3DLayer());
		PushOverlay(new PathfinderLayer());
	}

	~Sandbox()
	{}
};

Engine::Application* Engine::CreateApplication()
{
	return new Sandbox();
}
