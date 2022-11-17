/* The game engine series by The Cherno on Youtube, was a helpful source when creating this engine.
https://www.youtube.com/watch?v=JxIZbV_XjAs&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT&index=1 */

#include <Engine.h>

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class ExampleLayer : public Engine::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_CameraController(1280.0f / 720.0f, true)
	{
		m_VertexArray.reset(Engine::VertexArray::Create());


		float vertices[3 * 7] =
		{
		//	-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
		//	 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
		//	 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		std::shared_ptr<Engine::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Engine::VertexBuffer::Create(vertices, sizeof(vertices)));
		Engine::BufferLayout layout =
		{
			{ Engine::ShaderDataType::Float3, "a_Position" },
			{ Engine::ShaderDataType::Float4, "a_Color" }
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<Engine::IndexBuffer> indexBuffer;
		indexBuffer.reset(Engine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(Engine::VertexArray::Create());

		float squareVertices[5 * 4] =
		{
		//    x      y	     z?			uv
			-0.5f, -0.5f,   0.0f,	0.0f, 0.0f,	//	Bottom	- Left 
			 0.5f, -0.5f,   0.0f,	1.0f, 0.0f, //	Bottom	- Right
			 0.5f,  0.5f,   0.0f,	1.0f, 1.0f, //	Top		- Right
			-0.5f,  0.5f,   0.0f,	0.0f, 1.0f	//	Top		- Left
		};

		std::shared_ptr<Engine::VertexBuffer> SquareVB;
		SquareVB.reset(Engine::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		SquareVB->SetLayout
		({
			{ Engine::ShaderDataType::Float3, "a_Position" },
			{ Engine::ShaderDataType::Float2, "a_TexCoord" }
			});
		m_SquareVA->AddVertexBuffer(SquareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<Engine::IndexBuffer> SquareIB;
		SquareIB.reset(Engine::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(SquareIB);

		//--------------------------ObjLoader Test-----------------------------
		/* Loading obj */
		std::vector<glm::vec3> objVertices;
		std::vector<uint32_t> objIndices;
		Engine::ObjLoader::Get()->ReadFile("CubeCube", objVertices, objIndices);

		/* Creating vertex buffer */
		std::shared_ptr<Engine::VertexBuffer> objVB;
		std::vector<float> data;
		for (auto& it : objVertices) {
			data.push_back(*glm::value_ptr(it));
		}
		objVB.reset(Engine::VertexBuffer::Create(data.data(), sizeof(objVertices)));
		objVB->SetLayout(
			{
				{ Engine::ShaderDataType::Float3, "a_Position" },
			});

		/* Creating vertex array & adding vertex buffer*/
		m_objVA.reset(Engine::VertexArray::Create());
		m_objVA->AddVertexBuffer(objVB);

		/* Creating index buffer */
		std::shared_ptr<Engine::IndexBuffer> objIB;
		objIB.reset(Engine::IndexBuffer::Create(objIndices.data(), sizeof(objIndices) / sizeof(uint32_t)));
		m_objVA->SetIndexBuffer(objIB);
		//--------------------END ObjLoader Test -----------------------------------

		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ProjectionView;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ProjectionView * u_Transform * vec4(a_Position, 1.0);
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
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ProjectionView * u_Transform * vec4(a_Position, 1.0);
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
	}

	void OnUpdate(Engine::Timestep ts) override
	{
		// Update
		m_CameraController.OnUpdate(ts);

		// Render
		Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Engine::RenderCommand::Clear();

		Engine::Renderer::BeginScene(m_CameraController.GetCamera());

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
		Engine::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		Engine::Renderer::Submit(textureShader, m_objVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		m_WolfLogoTexture->Bind();
		Engine::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		//Engine::Renderer::Submit(m_Shader, m_VertexArray);

		Engine::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnEvent(Engine::Event& e) override
	{
		m_CameraController.OnEvent(e);
	}

private:
	Engine::ShaderLibrary m_ShaderLibrary;
	std::shared_ptr<Engine::Shader> m_Shader;
	std::shared_ptr<Engine::VertexArray> m_VertexArray;

	std::shared_ptr<Engine::Shader> m_FlatColorShader;
	std::shared_ptr<Engine::VertexArray> m_SquareVA;

	std::shared_ptr<Engine::VertexArray> m_objVA;	// For obj loader

	std::shared_ptr<Engine::Texture2D> m_Texture, m_WolfLogoTexture;

	Engine::OrthographicCameraController m_CameraController;
	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
};

class Pathfinder : public Engine::Layer
{
public:
	Pathfinder();
	~Pathfinder();


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
