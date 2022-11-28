#pragma once
#include <Engine.h>

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Engine/Renderer/VertexArray.h"
#include "Engine/AssetInit/ObjLoader.h"

class PathfindingLayer : public Engine::Layer
{
private:
	Engine::ShaderLibrary m_ShaderLibrary;
	std::shared_ptr<Engine::Shader> m_Shader, m_Textureshader;
	std::shared_ptr<Engine::Texture2D> m_Texture;

	Engine::PerspectiveCameraController m_PCameraController;

private:
	/* Test VisualObject with Pathfinding */
	//std::shared_ptr<Engine::VisualObject> m_Obj;
	
	std::shared_ptr<Engine::VertexArray> m_VA;
	std::shared_ptr<Engine::VertexArray> m_PlaneVA;
	std::shared_ptr<Engine::Scene> m_Scene;
	Engine::Entity m_Entity;
	Engine::Entity m_Plane;

	std::vector<glm::vec3> m_Path;
	std::vector<glm::vec3> m_SplinePath;
	float m_Splinetime{};
	float m_SplineSpeed{ 1.f };
	bool bObjPathfindActive{};

private: // Visual Aid & ImGui related elements
	bool bShowShaderColor{ true };
	std::shared_ptr<Engine::PNode> m_StartNode;
	std::shared_ptr<Engine::PNode> m_TargetNode;
	std::vector<std::shared_ptr<Engine::PNode>> m_BlockedNodes;

public:
	PathfindingLayer()
		: Layer("PathfindingLayer"), m_PCameraController(50.0f, 1280.0f / 720.0f, 0.01f, 1000.0f)
	{
		E_TRACE("float: {0}", sizeof(float));
		E_TRACE("glm::vec3: {0}", sizeof(glm::vec3));
		E_TRACE("glm::mat4: {0}", sizeof(glm::mat4));
		E_TRACE("NodeGrid: {0}", sizeof(Engine::NodeGrid));
		E_TRACE("PNode: {0}", sizeof(Engine::PNode));
		E_TRACE("TransformComponent: {0}", sizeof(Engine::TransformComponent));
		E_TRACE("PathfindingComponent: {0}", sizeof(Engine::PathfindingComponent));

		/* Pathfinding */
		//Engine::Pathfinder::SpawnGrid();
		//Engine::NodeGridSystem::CreateGridAtLocation(glm::vec3(0,0,0), 1.f, 10);
		Engine::NodeGridSystem::CreateGrid();

		m_Shader = m_ShaderLibrary.Load("assets/shaders/Flat.glsl");
		m_Textureshader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");
		m_Texture = Engine::Texture2D::Create("assets/textures/checkerboard.png");

		m_Textureshader->Bind();
		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Textureshader)->UploadUniformInt("u_Texture", 0);

		/* Loading obj */
		std::vector<Engine::Vertex> vertices;
		std::vector<uint32_t> indices;

		Engine::ObjLoader::ReadFile("Monkey", vertices, indices);
		//m_Obj = std::make_shared<Engine::VisualObject>(vertices, indices);

		m_Scene = std::make_shared<Engine::Scene>();
		m_Entity = Engine::EntityInitializer::GetInstance().EntityInit("Monkey", m_VA, m_Scene);
		m_Entity.AddComponent<Engine::RendererComponent>();
		m_Entity.AddComponent<Engine::PathfindingComponent>();

		/* Set m_Entity location to Pathfinding Node 1 */
		//glm::vec3 startPosition = Engine::Pathfinder::GetPositionofNode(0);
		//Engine::TransformSystem::SetWorldPosition(m_Entity.GetComponent<Engine::TransformComponent>(), startPosition);
		//m_Entity.GetComponent<Engine::PathfindingComponent>().m_StartNode = Engine::Pathfinder::GetNodeAtIndex(0);
		std::shared_ptr<Engine::PNode> startNode = Engine::NodeGridSystem::GetNodeAtIndexWithinGrid(0, 0);
		glm::vec3 startPosition = startNode->m_Position;
		Engine::TransformSystem::SetWorldPosition(m_Entity.GetComponent<Engine::TransformComponent>(), startPosition);
		m_Entity.GetComponent<Engine::PathfindingComponent>().m_StartNode = startNode;
		m_Entity.GetComponent<Engine::PathfindingComponent>().m_Grid = Engine::NodeGridSystem::GetGridAtIndex(0);

		m_Plane = Engine::EntityInitializer::GetInstance().EntityInit("Plane", m_PlaneVA, m_Scene);
		m_Plane.AddComponent<Engine::RendererComponent>();
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


		// PATHFINDING: Moving m_Entity along path
		auto& transform = m_Entity.GetComponent<Engine::TransformComponent>();
		auto& pathfinder = m_Entity.GetComponent<Engine::PathfindingComponent>();
		transform.m_Speed = m_SplineSpeed;
		if (pathfinder.bStartedPathfinding)
			Engine::PathfindingSystem::MoveAlongPath(pathfinder, transform, ts);
		
		//m_Splinetime = std::clamp<float>(m_Splinetime += (ts * m_SplineSpeed), 0.f, 1.f)/* / (float)Engine::Pathfinder::m_NodeLocations.size()*/;
		//if (m_Splinetime >= 1.f) {
		//	// Set Entity StartNode to TargetNode
		//	if (bObjPathfindActive) {
		//		auto& pathfinder = m_Entity.GetComponent<Engine::PathfindingComponent>();
		//		pathfinder.m_StartNode = pathfinder.m_TargetNode;
		//		E_INFO("Reached Node");
		//	}
		//	bObjPathfindActive = false; 
		//}
		//auto& transform = m_Entity.GetComponent<Engine::TransformComponent>();
		//if (bObjPathfindActive) {
		//	glm::vec3 pos = Engine::Pathfinder::GetPositionAlongSpline(m_Path, m_Splinetime);

		//	Engine::TransformSystem::SetWorldPosition(transform, pos + glm::vec3(0, 1, 0));
		//}


		// Changing color of the red channel - is multiplied in m_Shader's vertex shader 
		static float sin{};
		sin += ts;
		float tmp = sinf(sin);
		float fleeting = (sinf(sin) + 1) / 2;


		glm::vec4 nodeColor(0.1, 0.5, 0.1, 1);
		glm::vec4 startColor(1, 0, 0, 1);
		glm::vec4 targetColor(0, 0, 1, 1);
		glm::vec4 blockColor(0, 0, 0, 1);

		/* --------------------- RENDERING ------------------------ */
		m_Texture->Bind();
		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->Bind();

		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->UploadUniformFloat4("u_Color", glm::vec4(.7, .1, .6, 1));
		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->UploadUniformFloat3("u_Color", glm::vec4(.7, .1, .6, 1));
		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->UploadUniformInt("u_ShowCustomColor", false);

		Engine::TransformSystem::UpdateMatrix(transform);
		Engine::Renderer::Submit(m_Shader, m_VA, transform.m_Transform);		// Render m_Obj

		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->UploadUniformInt("u_ShowCustomColor", bShowShaderColor);
		/* ------- RENDER NODEGRID -------- */
		//auto& pathfinder = m_Entity.GetComponent<Engine::PathfindingComponent>();

		float scale = 0.3f;
		bool alteredColor{};
		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->UploadUniformFloat3("u_Color", nodeColor);
		//for (auto& it : Engine::Pathfinder::m_Nodes)
		for (auto& it : Engine::NodeGridSystem::GetGridAtIndex(0)->m_Nodes)
		{
			glm::vec3 position = it->m_Position;
			//if (it == m_StartNode) {
			if (it == pathfinder.m_StartNode) {
				std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->UploadUniformFloat3("u_Color", startColor);
				alteredColor = true;
			}
			else if (it == m_TargetNode) {
				std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->UploadUniformFloat3("u_Color", targetColor);
				alteredColor = true;
			}
			else if (it->IsBlock()) {
				std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->UploadUniformFloat3("u_Color", blockColor);
				alteredColor = true;
			}

			//Engine::Renderer::Submit(m_Shader, m_Obj->GetVertexArray(), glm::scale(glm::mat4(1.f), glm::vec3(scale)) * glm::translate(glm::mat4(1.f), glm::vec3(position / scale)));
			Engine::Renderer::Submit(m_Shader, m_PlaneVA, glm::scale(glm::mat4(1.f), glm::vec3(scale)) * glm::translate(glm::mat4(1.f), glm::vec3(position / scale)));
			if (alteredColor) {
				std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->UploadUniformFloat3("u_Color", nodeColor);
				alteredColor = false;
			}
		}

		/* ----- RENDER SPLINE PATH ----- */
		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->UploadUniformFloat3("u_Color", glm::vec3(1, 1, 1));
		scale = 0.1f;
		//for (auto& it : m_Path)
		for (auto& it : pathfinder.m_SplinePath->m_Controlpoints)
		{
			//Engine::Renderer::Submit(m_Shader, m_Obj->GetVertexArray(), glm::scale(glm::mat4(1.f), glm::vec3(scale)) * glm::translate(glm::mat4(1.f), glm::vec3(it / scale) + glm::vec3(0, 1.f, 0)));
			Engine::Renderer::Submit(m_Shader, m_PlaneVA, glm::scale(glm::mat4(1.f), glm::vec3(scale)) * glm::translate(glm::mat4(1.f), glm::vec3(it / scale) + glm::vec3(0, 0.2f, 0)));
		}

		// End Render Scene
		Engine::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::ShowDemoWindow();

		ImGui::Begin("Pathfinder");

		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted("RED: click to select start node\nBLUE: hold SHIFT to select target node\nGREY: hold ALT to set nodes to block");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
		
		// ----------------------------------------- NODE MENU BEGIN ------------------------------------------------------------------------
		/* Start and Target node selection */
		static int startSelected[100];
		static int startSelectedLocation{ -1 };

		static int targetSelected[100];
		static int targetSelectedLocation{ -1 };
		/* Blocked Nodes */
		static int blockSelected[100];

		static int selected[100];
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.5, 0.5, 0.5, 1));
		for (int y = 0; y < 10; y++)
			for (int x = 0; x < 10; x++)
			{
				if (x > 0)
					ImGui::SameLine();
				int location = x * 10 + y;
				//std::string name = Engine::Pathfinder::m_Nodes[location]->m_name;
				std::string name = Engine::NodeGridSystem::GetNodeAtIndexWithinGrid(0, location)->m_name;
				name.insert(5, "\n");


				// Set Block
				if (ImGui::IsKeyDown(ImGuiKey_LeftAlt)) {
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.2f, 0.2f, 0.2f, 1.f));
					ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.5f, 0.5f, 0.5f, 1.f));

				}
				// Set Start
				else if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.6f, 0.1f, 0.1f, 1.f));
					ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(1.f, 0.3f, 0.3f, 1.f));
				}
				// Set Target
				else {
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.1f, 0.1f, 0.6f, 1.f));
					ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.3f, 0.3f, 1.f, 1.f));
				}

				bool bHeader{};
				if (blockSelected[location]) {
					ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.4f, 0.4f, 0.4f, 1.f));
					bHeader = true;
				}
				else if (targetSelected[location]) {
					ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 1.f, 1.f));
					bHeader = true;
				}
				else if (startSelected[location]) {
					ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(1.f, 0.1f, 0.1f, 1.f));
					bHeader = true;
				}

				if (ImGui::Selectable(name.c_str(), selected[location] != 0, 0, ImVec2(50.f, 50.f)))
				{
					//std::shared_ptr<Engine::PNode> node = Engine::Pathfinder::m_Nodes[location];
					std::shared_ptr<Engine::PNode> node = Engine::NodeGridSystem::GetNodeAtIndexWithinGrid(0, location);
					selected[location] = 1;
					// Set a block Node
					if (ImGui::IsKeyDown(ImGuiKey_LeftAlt)) 
					{
						if (blockSelected[location]) {
							blockSelected[location] = 0;
							node->SetBlock(false);
							auto it = std::find(m_BlockedNodes.begin(), m_BlockedNodes.end(), node);
							m_BlockedNodes.erase(it);
						}
						else {
							blockSelected[location] = 1;
							node->SetBlock(true);	
							m_BlockedNodes.push_back(node);
						}
					}
					// Set Start Node
					else if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) 
					{
						startSelected[location] = 1;
						if (location != startSelectedLocation) {
							startSelected[startSelectedLocation] = 0;
						}
						startSelectedLocation = location;
						m_StartNode = node;
					}
					// Set Target Node
					else
					{
						targetSelected[location] = 1;
						if (location != targetSelectedLocation && targetSelectedLocation) {
							targetSelected[targetSelectedLocation] = 0;
						}
						targetSelectedLocation = location;
						m_TargetNode = node;
					}

					// Remove selected at location if in-active
					for (int i = 0; i < 100; i++)
						selected[i] = blockSelected[i];
					if (startSelectedLocation != -1)
						selected[startSelectedLocation] = 1;
					if (targetSelectedLocation != -1)
						selected[targetSelectedLocation] = 1;
				}
				if (bHeader)
					ImGui::PopStyleColor();

				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
			}
		ImGui::PopStyleColor();
		// NODE MENU END ---------------------------------------------------------------------------------


		// CLEAR BLOCKED NODE ----------------------------------------------------------------------------
		if (ImGui::Button("Clear Blocked Nodes", ImVec2(100.f, 20.f))) {
			for (auto& it : blockSelected)
				it = 0;
			for (int i = 0; i < 100; i++)
				selected[i] = blockSelected[i];
			for (auto& it : m_BlockedNodes)
				it->SetBlock(false);
			selected[startSelectedLocation] = 1;
			selected[targetSelectedLocation] = 1;

			m_BlockedNodes.clear();
		}

		// FINDING PATH ---------------------------------------------------------------------------------
		static std::vector<Engine::PNode*> nodepath;
		if (ImGui::Button("Find path", ImVec2(100,50))) {
			//if (m_StartNode.get() && m_TargetNode.get()) {
			if (m_TargetNode.get())
			{
				nodepath.clear();
				m_Path.clear();
				//nodepath = Engine::Pathfinder::FindPath(m_StartNode, m_TargetNode);
				auto& pathfinder = m_Entity.GetComponent<Engine::PathfindingComponent>();
				auto& transform = m_Entity.GetComponent<Engine::TransformComponent>();
				pathfinder.m_TargetNode = m_TargetNode;

				// Check if the Entity should not move
				const bool b = (m_TargetNode == pathfinder.m_StartNode);


				if (!b) {
					if (pathfinder.bStartedPathfinding)
						pathfinder.m_StartNode = Engine::PathfindingSystem::GetNodeClosestToPosition(0, transform.GetPosition());
					Engine::PathfindingSystem::FindPath(pathfinder);
					pathfinder.bStartedPathfinding = true;
				//if (bObjPathfindActive) {
				//	pathfinder.m_StartNode = Engine::Pathfinder::GetNodeClosestToPosition(transform.GetPosition());
				//}
					//if (!pathfinder.bReachedTarget)
						//pathfinder.m_StartNode = Engine::PathfindingSystem::GetNodeClosestToPosition(0, transform.GetPosition());
				//nodepath = Engine::Pathfinder::FindPath(pathfinder.m_StartNode, m_TargetNode);	// Finding path through node grid
				//m_Path.push_back(pathfinder.m_StartNode->m_Position);
				//for (auto it = nodepath.end(); it != nodepath.begin();) {
				//	m_Path.push_back((*--it)->m_Position);
				//}
				//// Make path a minimum of 3 points, for
				//if (m_Path.size() == 2) {
				//	glm::vec3 middle = m_Path[0] + ((m_Path[1] - m_Path[0]) * 0.5f);
				//	m_Path.insert(++m_Path.begin(), middle);
				//}

				//Engine::Pathfinder::MakeKnotVector(m_Path);
				//m_Splinetime = 0.f;
				//bObjPathfindActive = true;
				}
			}
		}
		ImGui::Separator();

		ImGui::PushItemWidth(150.f);
		ImGui::SliderFloat("Spline Speed", &m_SplineSpeed, 0.f, 2.f, "%0.1f");

		ImGui::End();
	}
};
