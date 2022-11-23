#pragma once
#include <Engine.h>

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Engine/Objects/VisualObject.h"
#include "Engine/AssetLoaders/ObjLoader.h"



class PathfindingLayer : public Engine::Layer
{
private:
	Engine::ShaderLibrary m_ShaderLibrary;
	std::shared_ptr<Engine::Shader> m_Shader, m_Textureshader;
	std::shared_ptr<Engine::Texture2D> m_Texture;

	Engine::PerspectiveCameraController m_PCameraController;
	//Engine::OrthographicCameraController m_OCameraController;

	/* Test VisualObject with Pathfinding */
	std::shared_ptr<Engine::VisualObject> m_Obj;
	std::vector<glm::vec3> m_Path;
	std::vector<glm::vec3> m_SplinePath;
	float m_Splinetime{};
	bool bObjPathfindActive{};
	bool bOverrideShaderColor{ true };
public:
	PathfindingLayer()
		: Layer("New3DLayer"), /*m_OCameraController(1280.0f / 720.0f, true),*/ m_PCameraController(50.0f, 1280.0f / 720.0f, 0.01f, 1000.0f)
	{
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
			Engine::Renderer::Submit(m_Shader, m_Obj->GetVertexArray(), glm::scale(glm::mat4(1.f), glm::vec3(scale)) * glm::translate(glm::mat4(1.f), glm::vec3(it / scale)));
		}

		/* Render Spline Path */
		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->UploadUniformFloat4("u_Color", color);
		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->UploadUniformInt("u_Override", bOverrideShaderColor);
		scale = 0.05f;
		for (auto& it : m_Path)
		{
			Engine::Renderer::Submit(m_Shader, m_Obj->GetVertexArray(), glm::scale(glm::mat4(1.f), glm::vec3(scale)) * glm::translate(glm::mat4(1.f), glm::vec3(it / scale) + glm::vec3(0, 0.1f, 0)));
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
		ImGui::ShowDemoWindow();

		ImGui::End();
	}
};
