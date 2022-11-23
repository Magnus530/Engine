/* The game engine series by The Cherno on Youtube, was a helpful source when creating this engine.
https://www.youtube.com/watch?v=JxIZbV_XjAs&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT&index=1 */

#include <Engine.h>

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Engine/Objects/VisualObject.h"
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
		SquareVB->SetLayout
		({
			{ Engine::ShaderDataType::Float3, "a_Position" },
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

		std::dynamic_pointer_cast<Engine::OpenGLShader>(flatShader)->UploadUniformFloat3("u_Color", m_SquareColor);

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

class New3DLayer : public Engine::Layer
{
private:
	Engine::ShaderLibrary m_ShaderLibrary;
	std::shared_ptr<Engine::Shader> m_Shader, m_Textureshader;
	std::shared_ptr<Engine::Texture2D> m_Texture;

	Engine::PerspectiveCameraController m_PCameraController;
	Engine::OrthographicCameraController m_OCameraController;

	/* Test VisualObject with Pathfinding */
	std::shared_ptr<Engine::VisualObject> m_Obj;
	std::vector<glm::vec3> m_Path;
	std::vector<glm::vec3> m_SplinePath;
	float m_Splinetime{};
	bool bObjPathfindActive{};
	bool bOverrideShaderColor{};

	/* Visual Point */
	//std::shared_ptr<Engine::VertexArray> m_PointVA;
	//std::vector<Engine::Vertex> m_PointVertices;
public:
	New3DLayer()
		: Layer("New3DLayer"), m_OCameraController(1280.0f / 720.0f, true), m_PCameraController(50.0f, 1280.0f / 720.0f, 0.01f, 1000.0f)
	{

		auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = Engine::Texture2D::Create("assets/textures/checkerboard.png");

		std::dynamic_pointer_cast<Engine::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<Engine::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);

		std::string vertexSrc = R"(
			#version 410 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec3 a_Normal;
			layout(location = 2) in vec2 a_TexCoord;

			out vec4 color;
			
			uniform mat4 u_ProjectionView;
			uniform mat4 u_ViewMatrix;
			uniform mat4 u_Transform;

			uniform vec4 u_Color;
			uniform int u_Override;

			void main()
			{
				color = vec4(a_Normal, 1);
				if (u_Override == 1){
					color = u_Color;
				}
				gl_Position = u_ProjectionView * u_ViewMatrix * u_Transform * vec4(a_Position, 1);
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
		m_Textureshader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");
		m_Texture = Engine::Texture2D::Create("assets/textures/checkerboard.png");

		m_Textureshader->Bind();
		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Textureshader)->UploadUniformInt("u_Texture", 0);

		/* Loading obj */
		std::vector<Engine::Vertex> vertices;
		std::vector<uint32_t> indices;

		Engine::ObjLoader::ReadFile("Plane", vertices, indices);
		m_Obj = std::make_shared<Engine::VisualObject>(vertices, indices);

		/* Pathfinding */
		Engine::Pathfinder::SpawnGrid();

		// Drawing a debug point
		//m_PointVA.reset(Engine::VertexArray::Create());
		//std::shared_ptr<Engine::VertexBuffer> vb;
		//for (auto& it : m_Path)
		//{
		//	m_PointVertices.emplace_back(it, glm::vec3(1,1,1 ));
		//}
		//vb.reset(Engine::VertexBuffer::Create(m_PointVertices.data(), m_PointVertices.size() * sizeof(Engine::Vertex)));
		//vb->SetLayout({
			//{ Engine::ShaderDataType::Float3, "PositionIn" },
			//{ Engine::ShaderDataType::Float3, "colorIn" },
			//{ Engine::ShaderDataType::Float2, "uvIn" }
			//});
		//m_PointVA->AddVertexBuffer(vb);
		//std::shared_ptr<Engine::IndexBuffer> ib;
		//ib.reset(Engine::IndexBuffer::Create(indices.data(), indices.size()));
		//m_PointVA->SetIndexBuffer(ib);
	}

	void OnUpdate(Engine::Timestep ts) override
	{
		// Update 
		m_PCameraController.OnUpdate(ts);

		// Begin Render Scene
		Engine::Renderer::BeginScene(m_PCameraController.GetCamera());
		
		// Set background color 
		Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Engine::RenderCommand::Clear();


		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		auto textureShader = m_ShaderLibrary.Get("Texture");

		m_Texture->Bind();

		m_SquareVA.reset(Engine::VertexArray::Create()); // OpenGLVertexArray*


		// Render Objects
		glm::mat4 projectionmatrix = m_PCameraController.GetCamera().GetProjectionMatrix();
		glm::mat4 viewmatrix = m_PCameraController.GetCamera().GetViewMatrix();

		if (m_Splinetime < 0.99f) m_Splinetime += (ts * 1.f)/* / (float)Engine::Pathfinder::m_NodeLocations.size()*/;
		else { m_Splinetime = 0.99f; bObjPathfindActive = false; }
		if (m_Splinetime > 0.99f) m_Splinetime = 0.99f;

		if (bObjPathfindActive) {
			glm::vec3 pos = Engine::Pathfinder::GetPositionAlongSpline(m_Path, m_Splinetime);
			//E_TRACE("SplineTime: {0}, {1}, {2}", pos.x, pos.y, pos.z);
			m_Obj->SetWorldPosition(pos);
		}
		// Changing color of the red channel - is multiplied in m_Shader's vertex shader 
		static float sin{};
		sin += ts;
		float tmp = sinf(sin);
		float fleeting = (sinf(sin) + 1) / 2;
		glm::vec4 color(1, 1, 1, 1);


		/* --------------------- RENDERING ------------------------ */
		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->UploadUniformInt("u_Override", 0);
		m_Texture->Bind();
		// SandboxApp.cpp
		// Draw call for Vertex array m_VA with m_Obj's position
		Engine::Renderer::Submit(m_Shader, m_Obj->GetVertexArray(), m_Obj->GetMatrix());		// Render m_Obj

		/* Render Nodegrid */
		float scale = 0.1f;
		for (auto& it : Engine::Pathfinder::m_NodeLocations)
		{
			Engine::Renderer::Submit(m_Shader, m_Obj->GetVertexArray(), glm::scale(glm::mat4(1.f), glm::vec3(scale)) * glm::translate(glm::mat4(1.f), glm::vec3(it/scale)));	
		}

		/* Render Spline Path */
		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->UploadUniformFloat4("u_Color", color);
		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->UploadUniformInt("u_Override", bOverrideShaderColor);
		scale = 0.05f;
		for (auto& it : m_Path)
		{
			Engine::Renderer::Submit(m_Shader, m_Obj->GetVertexArray(), glm::scale(glm::mat4(1.f), glm::vec3(scale)) * glm::translate(glm::mat4(1.f), glm::vec3(it / scale) + glm::vec3(0,0.1f,0)));
		}
		/* Render points */
		//Engine::Renderer::Submit(m_Shader, m_PointVA, glm::mat4(1.f));


		// TODO: Note regarding rendering 
		/* Want it to be something like this. 
		* The scene tells the renderer which object to render.
		* The renderer, which will then hold all vertex arrays and buffer, will check which arrays and buffers belong to that object
		* If the buffers do not exist, create them and do a single render call.
		*	else if the buffers exists, do instance rendering */
		//Engine::Renderer::Submit(m_Shader, m_Obj); // Something like this

		// End Render Scene
		Engine::Renderer::EndScene();
	}

private:
	Engine::ShaderLibrary m_ShaderLibrary;
	std::shared_ptr<Engine::Shader> m_Shader;
	std::shared_ptr<Engine::vShader> m_vShader;
	
	std::unique_ptr<Engine::VisualObject> m_Obj;
	std::shared_ptr<Engine::Texture2D> m_Texture;

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

		/* Override Shader Color */
		ImGui::Checkbox("OverrideShaderColor", &bOverrideShaderColor);
		//ImGui::Colo

		/* Pathfinding GUI */
		using namespace Engine;	// Tmp, for readability

		static std::string currentStart{ NULL };
		static std::string currentEnd{ NULL };
		static std::shared_ptr<PNode> start;
		static std::shared_ptr<PNode> end;
		static std::vector<PNode*> nodepath;
		if (ImGui::BeginCombo("StartNode", currentStart.c_str())) {
			for (const auto& it : Pathfinder::m_Nodes)
			{
				if (ImGui::Selectable(it->m_name.c_str())) {
					currentStart = it->m_name;
					start = it;
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::BeginCombo("EndNode", currentEnd.c_str())) {
			for (const auto& it : Pathfinder::m_Nodes)
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
				nodepath.clear();
				m_Path.clear();
				nodepath = Pathfinder::FindPath(start, end.get());
				m_Path.push_back(start->m_Position);
				for (auto it = nodepath.end(); it != nodepath.begin();) {
					m_Path.push_back((*--it)->m_Position);
				}
				//m_Path.push_back(start->m_Position);
				Pathfinder::MakeKnotVector(m_Path);
				m_Splinetime = 0.f;
				bObjPathfindActive = true;
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
	}

	~Sandbox()
	{}
};

Engine::Application* Engine::CreateApplication()
{
	return new Sandbox();
}
