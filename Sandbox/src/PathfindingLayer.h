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
	std::shared_ptr<Engine::VertexArray> m_BeveledCubeVA;
	std::shared_ptr<Engine::Scene> m_Scene;
	Engine::Entity m_Entity;
	Engine::Entity m_Plane;

	std::vector<glm::vec3> m_Path;
	std::vector<glm::vec3> m_SplinePath;
	float m_Splinetime{};
	float m_SplineSpeed{ 1.f };
	bool bObjPathfindActive{};

	//Engine::Entity m_Obstructor;
	std::vector<Engine::Entity> m_Obstructors;

	//uint32_t m_ObstructionSphere;

private: // Visual Aid & ImGui related elements
	bool bShowShaderColor{ true };
	std::shared_ptr<Engine::PNode> m_StartNode;
	std::shared_ptr<Engine::PNode> m_TargetNode;
	std::vector<std::shared_ptr<Engine::PNode>> m_BlockedNodes;


public:
	PathfindingLayer()
		: Layer("PathfindingLayer"), m_PCameraController(50.0f, 1280.0f / 720.0f, 0.01f, 1000.0f)
	{
		//E_TRACE("PathBlocking Sphere : {0}", sizeof(Engine::PathObstructionSphere));

		/* Pathfinding */
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

		m_Scene = std::make_shared<Engine::Scene>();
		m_Entity = Engine::EntityInitializer::GetInstance().EntityInit("Monkey", m_VA, m_Scene);
		m_Entity.AddComponent<Engine::RendererComponent>();
		m_Entity.AddComponent<Engine::PathfindingComponent>();

		/* Set m_Entity location to Pathfinding Node 1 */
		std::shared_ptr<Engine::PNode> startNode = Engine::NodeGridSystem::GetNodeAtIndexWithinGrid(0, 0);
		glm::vec3 startPosition = startNode->m_Data->m_Position;
		Engine::TransformSystem::SetWorldPosition(m_Entity.GetComponent<Engine::TransformComponent>(), startPosition + glm::vec3(0, 0.5f, 0)); //Manually adding extra height
		m_Entity.GetComponent<Engine::PathfindingComponent>().m_StartNode = startNode;
		m_Entity.GetComponent<Engine::PathfindingComponent>().m_Grid = Engine::NodeGridSystem::GetGridAtIndex(0);

		m_Plane = Engine::EntityInitializer::GetInstance().EntityInit("Plane", m_PlaneVA, m_Scene);
		m_Plane.AddComponent<Engine::RendererComponent>();

		// Creating Pathfinding Obstructions 
		InitVertexArray("BeveledCube", m_BeveledCubeVA);	// Bruker denne vertex arrayen flere ganger, så Initialiserer den for seg selv her

		CreateObstructor(glm::vec3(  5.f,  0.f,   5.f), 2.f);
		//CreateObstructor(glm::vec3( -5.f,  0.f,   5.f), 3.f);
	}

	//----------------------------------------------------------------UPDATE-------------------------------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------UPDATE-------------------------------------------------------------------------------------------------------------------------
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


		/*--------------------- PATHFINDING: Moving m_Entity along path -----------------------------*/
		auto& transform = m_Entity.GetComponent<Engine::TransformComponent>();
		auto& pathfinder = m_Entity.GetComponent<Engine::PathfindingComponent>();
		transform.m_Speed = m_SplineSpeed;
		if (pathfinder.bStartedPathfinding)
			Engine::PathfindingSystem::MoveAlongPath(pathfinder, transform, ts);

		Engine::NodeGridSystem::UpdateFalseObstructionNodes(0);
		/************************************************************************************************/

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

		/*-----------RENDER OBSTRUCTIONS---------------*/
		for (uint32_t i{}; i < m_Obstructors.size(); i++)
		{
			auto& transform2 = m_Obstructors[i].GetComponent<Engine::TransformComponent>();
			Engine::Renderer::Submit(m_Shader, m_BeveledCubeVA, transform2.m_Transform);
		}

		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->UploadUniformInt("u_ShowCustomColor", bShowShaderColor);
		/* ------- RENDER NODEGRID -------- */

		float scale = 0.3f;
		bool alteredColor{};
		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->UploadUniformFloat3("u_Color", nodeColor);
		for (auto& it : Engine::NodeGridSystem::GetGridAtIndex(0)->m_Nodes)
		{
			glm::vec3 position = it->m_Data->m_Position;
			if (it == pathfinder.m_StartNode) {
				std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->UploadUniformFloat3("u_Color", startColor);
				alteredColor = true;
			}
			else if (it == m_TargetNode) {
				std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->UploadUniformFloat3("u_Color", targetColor);
				alteredColor = true;
			}
			else if (it->IsObstruction()) {
				std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->UploadUniformFloat3("u_Color", blockColor);
				alteredColor = true;
			}

			Engine::Renderer::Submit(m_Shader, m_PlaneVA, glm::scale(glm::mat4(1.f), glm::vec3(scale)) * glm::translate(glm::mat4(1.f), glm::vec3(position / scale)));
			if (alteredColor) {
				std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->UploadUniformFloat3("u_Color", nodeColor);
				alteredColor = false;
			}
		}

		/* ----- RENDER SPLINE PATH ----- */
		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader)->UploadUniformFloat3("u_Color", glm::vec3(1, 1, 1));
		scale = 0.1f;
		for (auto& it : pathfinder.m_SplinePath->m_Controlpoints)
		{
			Engine::Renderer::Submit(m_Shader, m_PlaneVA, glm::scale(glm::mat4(1.f), glm::vec3(scale)) * glm::translate(glm::mat4(1.f), glm::vec3(it / scale) + glm::vec3(0, 0.2f, 0)));
		}

		// End Render Scene
		Engine::Renderer::EndScene();
	}

	//----------------------------------------------------------------IMGUI-------------------------------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------IMGUI-------------------------------------------------------------------------------------------------------------------------
	virtual void OnImGuiRender() override
	{
		ImGui::ShowDemoWindow();

		ImGui::Begin("Pathfinder");

		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted("LEFT CLICK: Set target\nRIGHT CLICK: Make node a path obstruction");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		// ----------------------------------------- NODE MENU BEGIN ------------------------------------------------------------------------
		// Target Nodes
		static int targetSelected[100];
		static int targetSelectedLocation{ -1 };
		// Blocked Nodes 
		static int blockSelected[100];

		static int selected[100];
		for (int y = 0; y < 10; y++)
			for (int x = 0; x < 10; x++)
			{
				if (x > 0)
					ImGui::SameLine();
				int location = x * 10 + y;
				std::string name{ Engine::NodeGridSystem::GetNodeAtIndexWithinGrid(0, location)->m_Data->m_Name };
				name.insert(5, "\n");

				//** SET STYLE
				// Set Block
				if (ImGui::IsMouseDown(1)) 
				{
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.6f, 0.6f, 0.6f, 1.f));
					ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.6f, 0.6f, 0.6f, 1.f));
				}
				// Set Target
				else 
				{
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.4f, 0.2f, 0.3f, 1.f));
					ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.7f, 0.3f, 0.5f, 1.f));
				}
				//*********

				bool bHeader{};
				if (blockSelected[location]) {
					ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.4f, 0.4f, 0.4f, 1.f));
					bHeader = true;
				}
				else if (targetSelected[location]) {
					ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 1.f, 1.f));
					bHeader = true;
				}

				// Creating the selectable grid 
				if (ImGui::Selectable(name.c_str(), selected[location] != 0, 0, ImVec2(50.f, 50.f)))
				{
					// Remove selected at location if in-active
					for (int i = 0; i < 100; i++)
						selected[i] = blockSelected[i];
					if (targetSelectedLocation != -1)
						selected[targetSelectedLocation] = 1;
				}

				//** Left click - Set target and start pathfinding
				if (ImGui::IsItemClicked(0)) {
					std::shared_ptr<Engine::PNode> node = Engine::NodeGridSystem::GetNodeAtIndexWithinGrid(0, location);

					targetSelected[location] = 1;
					if (location != targetSelectedLocation && targetSelectedLocation != -1) {
						targetSelected[targetSelectedLocation] = 0;
					}
					targetSelectedLocation = location;
					m_TargetNode = node;

					//** INIT PATHFINDING ******
					auto& pathfinder = m_Entity.GetComponent<Engine::PathfindingComponent>();
					auto& transform = m_Entity.GetComponent<Engine::TransformComponent>();
					pathfinder.m_TargetNode = m_TargetNode;
					Engine::PathfindingSystem::FindPath(pathfinder, transform.GetPosition() - glm::vec3(0, 0.5f, 0));
					//********
				}

				//** Right click - Set node to block 
				if (ImGui::IsItemClicked(1)) {
					std::shared_ptr<Engine::PNode> node = Engine::NodeGridSystem::GetNodeAtIndexWithinGrid(0, location);

					if (blockSelected[location]) {
						blockSelected[location] = 0;
						selected[location] = 0;
						node->SetObstructionStatus(false);
						auto it = std::find(m_BlockedNodes.begin(), m_BlockedNodes.end(), node);
						m_BlockedNodes.erase(it);
					}
					else {
						selected[location] = 1;
						blockSelected[location] = 1;
						node->SetObstructionStatus(true);
						m_BlockedNodes.push_back(node);
					}
				}

				if (bHeader)
					ImGui::PopStyleColor();

				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
			}
		// NODE MENU END ---------------------------------------------------------------------------------


		// CLEAR BLOCKED NODE ----------------------------------------------------------------------------
		//if (ImGui::Button("Clear Blocked Nodes", ImVec2(100.f, 20.f))) {
		//	for (auto& it : blockSelected)
		//		it = 0;
		//	for (int i = 0; i < 100; i++)
		//		selected[i] = blockSelected[i];
		//	for (auto& it : m_BlockedNodes)
		//		it->SetObstructionStatus(false);
		//	selected[targetSelectedLocation] = 1;
		//
		//	m_BlockedNodes.clear();
		//}

		ImGui::Separator();
		
		// SPLINE SPEED ------------------------------------------------------------------------------------
		ImGui::PushItemWidth(150.f);
		ImGui::SliderFloat("Spline Speed", &m_SplineSpeed, 0.f, 2.f, "%0.1f");
		



		// ADJUST OBSTRTUCTION ENTITIES ------------------------------------------------------------------------------------
		ImGui::Separator();
		if (ImGui::Button("CREATE\nObstruction", ImVec2(100.f, 100.f)))
			CreateObstructor();

		ImGui::SameLine();
		if (ImGui::Button("DELETE\nObstruction", ImVec2(100.f, 100.f)))
			DeleteObstructor();

		uint32_t size = m_Obstructors.size();
		//std::vector<glm::vec3> positions;
		//std::vector<bool> bRadius;
		//std::vector<bool> bPositionsX;
		//std::vector<bool> bPositionsZ;
		//for (uint32_t i{}; i < size; i++) {
		//	positions.push_back(glm::vec3(0.f));
		//	bRadius.push_back(false);
		//	bPositionsX.push_back(false);
		//	bPositionsZ.push_back(false);
		//}
		
		static auto ID = ImGui::GetActiveID();
		for (uint32_t i{}; i < size; i++)
		{
			ImGui::Separator();
			ImGui::PushID(i);
			//E_TRACE("ImGuiID {0}", ImGui::GetActiveID());
			auto& obstruction = m_Obstructors[i].GetComponent<Engine::ObstructionSphereComponent>();
			auto& transform = m_Obstructors[i].GetComponent<Engine::TransformComponent>();

			std::string text = "Obstruction " + std::to_string(obstruction.m_ID);
			ImGui::Text(text.c_str());
			ImGui::PushItemWidth(150.f);
			
			const bool r = ImGui::SliderFloat("Radius", &obstruction.m_radius, 0.f, 5.f, "%.1f");

			glm::vec3 pos = transform.GetPosition();
			ImGui::PushItemWidth(100.f);

			const bool x = ImGui::SliderFloat("X", &pos.x, -5.f, 5.f, "%0.1f");

			ImGui::SameLine();
			const bool z = ImGui::SliderFloat("Z", &pos.z, -5.f, 5.f, "%0.1f");
			//ImGui::GetActiveID();

			if (r || x || z) {
				ID = ImGui::GetActiveID();
				
				Engine::TransformSystem::SetWorldPosition(transform, pos);
				Engine::TransformSystem::UpdateMatrix(transform);
				//E_TRACE("radius {0}", obstruction.m_radius);
				Engine::NodeGridSystem::UpdateObstructionSphere(0, obstruction.m_ID, obstruction.m_radius, transform.GetPosition());
			}
			ImGui::PopID();
			ImGui::Separator();
		}

		if (ImGui::Button("UpdateTEST", ImVec2(100.f, 75.f)))
			for (uint32_t i{}; i < size; i++)
			{
				auto& obstruction = m_Obstructors[i].GetComponent<Engine::ObstructionSphereComponent>();
				auto& transform = m_Obstructors[i].GetComponent<Engine::TransformComponent>();
				Engine::NodeGridSystem::UpdateObstructionSphere(0, obstruction.m_ID, obstruction.m_radius, transform.GetPosition());
			}


		//if (at != -1)
		//{
		//	auto& obstruction = m_Obstructors[at].GetComponent<Engine::ObstructionSphereComponent>();
		//	auto& transform = m_Obstructors[at].GetComponent<Engine::TransformComponent>();
		//	Engine::TransformSystem::SetWorldPosition(transform, positions[at]);
		//	Engine::TransformSystem::UpdateMatrix(transform);
		//	Engine::NodeGridSystem::UpdateObstructionSphere(0, obstruction.m_ID, obstruction.m_radius, transform.GetPosition());
		//}
		//positions.clear();

		ImGui::End();
	}

	void InitVertexArray(std::string objname, std::shared_ptr<Engine::VertexArray>& vertexarr)
	{
		std::vector<Engine::Vertex> vertices;
		std::vector<uint32_t> indices;

		Engine::ObjLoader::ReadFile(objname, vertices, indices);
		vertexarr.reset(Engine::VertexArray::Create());
		std::shared_ptr<Engine::VertexBuffer> ObjVB;
		ObjVB.reset(Engine::VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(Engine::Vertex))); // OpenGLVertexBuffer*	// for en vector av floats
		ObjVB->SetLayout
		({
			{ Engine::ShaderDataType::Float3, "a_Position" },
			{ Engine::ShaderDataType::Float3, "a_Normal" },
			{ Engine::ShaderDataType::Float2, "a_TexCoord" }
			});
		vertexarr->AddVertexBuffer(ObjVB);

		std::shared_ptr<Engine::IndexBuffer> ObjIB;
		ObjIB.reset(Engine::IndexBuffer::Create(indices)); // OpenGLIndexBuffer*
		vertexarr->SetIndexBuffer(ObjIB);
	}

	void CreateObstructor() {
		CreateObstructor(glm::vec3(0.f));
	}
	void CreateObstructor(glm::vec3 pos, float radius = 2.f)
	{
		Engine::Entity ent = Engine::EntityInitializer::GetInstance().EntityInit("Obstructor " + std::to_string(m_Obstructors.size()-1), m_Scene);
		ent.AddComponent<Engine::RendererComponent>();
		ent.AddComponent<Engine::ObstructionSphereComponent>();
		auto& obs = ent.GetComponent<Engine::ObstructionSphereComponent>();
		obs.m_radius = radius;
		auto& t = ent.GetComponent<Engine::TransformComponent>();
		Engine::TransformSystem::SetWorldPosition(t, pos);
		obs.m_ID = Engine::NodeGridSystem::CreateObstructionSphere(0, obs.m_radius, t.GetPosition());
		
		m_Obstructors.push_back(ent);
	}
	void DeleteObstructor()
	{
		uint32_t size = m_Obstructors.size();
		if (size == 0) return;
		Engine::NodeGridSystem::DeleteObstructionSphere(0, size - 1);
		m_Obstructors.erase(--m_Obstructors.end());
	}
};
