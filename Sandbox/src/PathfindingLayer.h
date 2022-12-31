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
	std::shared_ptr<Engine::AudioEngine> m_Audio;
	Engine::PerspectiveCameraController m_PCameraController;

private:
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

	std::vector<Engine::Entity> m_Obstructors;

private: // Visual Aid & ImGui related elements
	bool bRenderNodeGrid{ true };
	bool bShowShaderColor{ true };
	int m_StartNode;
	int m_TargetNode;
	std::vector<int> m_BlockedNodes;

private: // Screen RayCasting
	glm::vec2 mousePos;
	//Engine::Entity m_BeveledCube;


public:
	PathfindingLayer()
		: Layer("PathfindingLayer"), m_PCameraController(50.0f, 1280.0f / 720.0f, 0.01f, 1000.0f)
	{
		/* Sizeof */
		E_CORE_TRACE("Sizeof: NodeGrid = {0}", sizeof(Engine::NodeGrid));
		E_CORE_TRACE("Sizeof: PathObstructionSphereCollection = {0}", sizeof(Engine::PathObstructionSphereCollection));
		E_CORE_TRACE("Sizeof: glm::vec3 = {0}", sizeof(glm::vec3));
		E_CORE_TRACE("Sizeof: std::vector<int> = {0}", sizeof(std::vector<int>));
		E_CORE_TRACE("Sizeof: std::vector<int>* = {0}", sizeof(std::vector<int>*));

		/* Pathfinding */
		Engine::NodeGridSystem::CreateGridAtLocation(glm::vec3(0,0,0), glm::vec3((int)20, 0, (int)20), 1);

		//m_Shader = m_ShaderLibrary.Load("assets/shaders/Flat.glsl");
		//m_Textureshader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");
		//m_Texture = Engine::Texture2D::Create("assets/textures/checkerboard.png");

		//m_Textureshader->Bind();
		//std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Textureshader)->UploadUniformInt("u_Texture", 0);

		/* Loading obj */
		std::vector<Engine::Vertex> vertices;
		std::vector<uint32_t> indices;

		Engine::ObjLoader::ReadFile("Monkey", vertices, indices);

		m_Scene = std::make_shared<Engine::Scene>();
		m_Entity = Engine::EntityInitializer::GetInstance().EntityInit(Engine::ShaderType::Flat, "Monkey", m_VA, m_Scene, glm::vec3(0.7, 0.4, 0.2));
		m_Entity.AddComponent<Engine::PathfindingComponent>();

		// Audio
		m_Audio = std::make_shared<Engine::AudioEngine>();

		/* Set m_Entity location to Pathfinding Node 1 */
		glm::vec3 startPosition = Engine::NodeGridSystem::GetNodeLocation(0, 0);
		Engine::TransformSystem::SetWorldPosition(m_Entity.GetComponent<Engine::TransformComponent>(), startPosition + glm::vec3(0, 0.5f, 0)); //Manually adding extra height
		Engine::TransformSystem::RotateToDirectionVector(m_Entity.GetComponent<Engine::TransformComponent>(), glm::normalize(glm::vec3(-1, 0, 1)));

		m_Entity.GetComponent<Engine::PathfindingComponent>().m_StartNode = 0;
		m_Entity.GetComponent<Engine::PathfindingComponent>().m_Grid = 0;


		// Vertex Array Pathfinding 
			// Obstructions
		InitVertexArray("BeveledCube", m_BeveledCubeVA);	// Bruker denne vertex arrayen flere ganger, s� Initialiserer den for seg selv her
			// Nodes
		InitVertexArray("Plane", m_PlaneVA);
	}

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

		m_Audio->update(ts);

		// Render Objects
		glm::mat4 projectionmatrix = m_PCameraController.GetCamera().GetProjectionMatrix();
		glm::mat4 viewmatrix = m_PCameraController.GetCamera().GetViewMatrix();


		/*--------------------- PATHFINDING: Moving m_Entity along path -----------------------------*/
		auto& transform = m_Entity.GetComponent<Engine::TransformComponent>();
		auto& pathfinder = m_Entity.GetComponent<Engine::PathfindingComponent>();
		Engine::PathfindingSystem::MoveAlongPath(pathfinder, transform, ts);
		Engine::NodeGridSystem::UpdateFalseObstructionNodes(0);


		/* --------------------- RENDERING ------------------------ */
		Engine::Renderer::Submit(m_Entity);
		
		/*-----------RENDER OBSTRUCTIONS---------------*/
		for (auto& it : m_Obstructors)
			Engine::Renderer::Submit(it);
		
#ifdef E_DEBUG /* ------- RENDER NODEGRID -------- */
		if (bRenderNodeGrid)
		{
			glm::vec4 nodeColor(0.1, 0.5, 0.1, 1);
			glm::vec4 startColor(1, 0, 0, 1);
			glm::vec4 targetColor(0, 0, 1, 1);
			glm::vec4 blockColor(0, 0, 0, 1);

			float scale = 0.1f;
			bool alteredColor{};
			auto grid = Engine::NodeGridSystem::GetGridAtIndex(0);
			for (size_t i{0}; i < grid->m_NodeLocations->size(); i++)
			{
				if (i == pathfinder.m_StartNode) {
					Engine::Renderer::Submit(m_PlaneVA, glm::vec3(grid->m_NodeLocations->at(i) / scale), scale, startColor);
				}
				else if (i == m_TargetNode) {
					Engine::Renderer::Submit(m_PlaneVA, glm::vec3(grid->m_NodeLocations->at(i) / scale), scale, targetColor);
				}
				else if (grid->m_NodeObstructionStatus->at(i)) {
					Engine::Renderer::Submit(m_PlaneVA, glm::vec3(grid->m_NodeLocations->at(i) / scale), scale, blockColor);
				}
				else {
					Engine::Renderer::Submit(m_PlaneVA, glm::vec3(grid->m_NodeLocations->at(i) / scale), scale, nodeColor);
				}
			}

			/* ----- RENDER SPLINE PATH ----- */
			scale /= 2.f;
			for (auto& it : pathfinder.m_SplinePath->m_Controlpoints)
				Engine::Renderer::Submit(m_PlaneVA, glm::vec3(it / scale) + glm::vec3(0, 0.2f, 0), scale, {1,1,1});
		}
#endif
		// End Render Scene
		Engine::Renderer::EndScene();
	}

	//----------------------------------------------------------------IMGUI-------------------------------------------------------------------------------------------------------------------------
	virtual void OnImGuiRender() override
	{
		//ImGui::ShowDemoWindow();

		ImGui::Begin("Pathfinder");
		
		ImGui::Separator();
		ImGui::PushID(0);
		ImGui::PushItemWidth(200.f);
		ImGui::Text("Monkey");
		ImGui::Checkbox("Show Normals", &m_Entity.GetComponent<Engine::RendererComponent>().m_bCustomColor);
		ImGui::PopID();

		// TransformTesting -  Rotate to vector
		static float x{ 1 };
		static float y{ 1 };
		static float z{ -1 };
		ImGui::PushItemWidth(100.f);
		for (size_t i{}; i < 3; i++)
		{
			ImGui::PushID(i);
			std::string c;
			float* v = nullptr;
			switch (i)
			{
			case 0:
				c = 'x';
				v = &x;
				break;
			case 1:
				c = 'y';
				v = &y;
				ImGui::SameLine();
				break;
			case 2:
				c = 'z';
				v = &z;
				ImGui::SameLine();
				break;
			default:
				break;
			}
			if (ImGui::SliderFloat(c.c_str(), v, -1.f, 1.f, "%.1f"))
			{
				ImGui::GetActiveID();
				Engine::TransformSystem::RotateToDirectionVector(m_Entity.GetComponent<Engine::TransformComponent>(), glm::vec3(x, y, z));
			}
			ImGui::PopID();
		}

		ImGui::Separator();
		ImGui::Text("Obstructors");
		ImGui::PushID(1);
		static glm::vec3 color{ 0.5,0.5,0.5 };
		static bool bObstructor_ShowNormal{};
		ImGui::ColorEdit3("", glm::value_ptr(color));
		if (ImGui::Checkbox("Show Normals", &bObstructor_ShowNormal))
			for (auto& it : m_Obstructors)
				it.GetComponent<Engine::RendererComponent>().m_bCustomColor = bObstructor_ShowNormal;
		if (!bObstructor_ShowNormal)
			for (auto& it : m_Obstructors)
				it.GetComponent<Engine::FlatMaterialComponent>().m_Color = glm::vec4(color, 1.f);
		ImGui::PopID();



		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted("Click within the nodegrid to move the monkey to that location");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
		ImGui::Separator();

		ImGui::Checkbox("Render Nodegrid", &bRenderNodeGrid);
		ImGui::Separator();
		

		// SPLINE SPEED ------------------------------------------------------------------------------------
		ImGui::PushItemWidth(150.f);
		ImGui::DragFloat("Monkey Movement Speed", &m_Entity.GetComponent<Engine::PathfindingComponent>().m_EntityPathSpeed, 0.2f, 0.0f, 1000.f, "%.1f");
		

		// PATROL ---------------------------------------------------------------
		auto& pathfinder = m_Entity.GetComponent<Engine::PathfindingComponent>();
		auto& transform = m_Entity.GetComponent<Engine::TransformComponent>();
		glm::vec3 currentPos = transform.GetPosition() - glm::vec3(0, 0.5, 0);

		if (ImGui::Button("Start\nPatrol", ImVec2(100, 100)))
			Engine::PathfindingSystem::StartPatrol(pathfinder, currentPos);
		ImGui::SameLine();
		if (ImGui::Button("Clear Patrol Path", ImVec2(90, 40)))
			Engine::PathfindingSystem::ClearPatrol(pathfinder);

		static bool bPatrolpaused{};
		if (ImGui::Button(bPatrolpaused ? "Resume Patrol" : "Exit Patrol", ImVec2(100, 70)))
		{
			if (!bPatrolpaused)
			{
				Engine::PathfindingSystem::PausePatrol(pathfinder, currentPos);
				//Engine::PathfindingSystem::CancelMovementAlongPath(pathfinder);
				bPatrolpaused = true;
			}
			else
			{
				Engine::PathfindingSystem::ResumePatrol(pathfinder, currentPos);
				//Engine::PathfindingSystem::ResumeMovementAlongPath(pathfinder, currentPos);
				bPatrolpaused = false;
			}
		}


		// ADJUST OBSTRTUCTION ENTITIES ------------------------------------------------------------------------------------
		ImGui::Separator();
		if (ImGui::Button("CREATE\nObstruction", ImVec2(100.f, 100.f)))
			CreateObstructor();

		ImGui::SameLine();
		if (ImGui::Button("DELETE\nObstruction", ImVec2(100.f, 100.f)))
			DeleteObstructor();

		size_t size = m_Obstructors.size();

		static auto ID = ImGui::GetActiveID();
		for (size_t i{}; i < size; i++)
		{
			ImGui::Separator();
			ImGui::PushID(i);
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

			if (r || x || z) {
				ID = ImGui::GetActiveID();
				
				Engine::TransformSystem::SetWorldPosition(transform, pos);
				Engine::NodeGridSystem::UpdateObstructionSphere(0, obstruction.m_ID, obstruction.m_radius, transform.GetPosition());
			}
			ImGui::PopID();
			ImGui::Separator();
		}


		ImGui::End();
	}

	//----------------------------------------------------------------ON EVENT-------------------------------------------------------------------------------------------------------------------------
	virtual void OnEvent(Engine::Event& e)
	{
		if (e.GetEventType() == Engine::EventType::MouseMoved)
		{
			Engine::MouseMovedEvent& event = (Engine::MouseMovedEvent&)e;
			mousePos.x = event.GetX();
			mousePos.y = event.GetY();
		}


		if (e.GetEventType() == Engine::EventType::MouseButtonPressed && Engine::Input::IsMouseButtonPressed(E_MOUSE_BUTTON_LEFT) && Engine::Input::IsKeyPressed(E_KEY_LEFT_ALT))
		{
			// Set Patrol point
			glm::vec3 ray{};
			Engine::RayCast::FromScreenPosition(ray, mousePos, m_PCameraController.GetCamera().GetProjectionMatrix(), m_PCameraController.GetCamera().GetViewMatrix());
			glm::vec3 pos = m_PCameraController.GetCamera().GetPosition();
			glm::vec3 Intersection;
			if (Engine::RayCast::IntersectWithWAPlaneXZ(Intersection, ray, pos))
			{
				m_Entity.GetComponent<Engine::PathfindingComponent>().m_PatrolPath.push_back(Intersection);
			}
		}
		else if (e.GetEventType() == Engine::EventType::MouseButtonPressed && Engine::Input::IsMouseButtonPressed(E_MOUSE_BUTTON_LEFT))
		{
			// Start Pathfinding - Find raycasted point on XZ plane - Node closest to location
			glm::vec3 ray{};
			Engine::RayCast::FromScreenPosition(ray, mousePos, m_PCameraController.GetCamera().GetProjectionMatrix(), m_PCameraController.GetCamera().GetViewMatrix());
			glm::vec3 pos = m_PCameraController.GetCamera().GetPosition();
			glm::vec3 Intersection;
			if (Engine::RayCast::IntersectWithWAPlaneXZ(Intersection, ray, pos)) 
			{
				auto& transform = m_Entity.GetComponent<Engine::TransformComponent>();
				m_StartNode = Engine::PathfindingSystem::GetNodeClosestToPosition(0, transform.GetPosition());
				m_TargetNode = Engine::PathfindingSystem::GetNodeClosestToPosition(0, Intersection);

				auto& pathfinder = m_Entity.GetComponent<Engine::PathfindingComponent>();
				pathfinder.m_TargetNode = m_TargetNode;
				Engine::PathfindingSystem::FindPath(pathfinder, transform.GetPosition() - glm::vec3(0, 0.5f, 0));
			}
		}

	}



	//----------------------------------------------------------------Smaller Specific Functions-------------------------------------------------------------------------------------------------------------------------
	void InitVertexArray(std::string objname, std::shared_ptr<Engine::VertexArray>& vertexarr)
	{
		std::vector<Engine::Vertex> vertices;
		std::vector<uint32_t> indices;

		Engine::ObjLoader::ReadFile(objname, vertices, indices);
		vertexarr.reset(Engine::VertexArray::Create());
		std::shared_ptr<Engine::VertexBuffer> ObjVB;
		ObjVB.reset(Engine::VertexBuffer::Create(vertices.data(), (uint32_t)vertices.size() * sizeof(Engine::Vertex))); // OpenGLVertexBuffer*	// for en vector av floats
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
		//Engine::Entity ent = Engine::EntityInitializer::GetInstance().EntityInit("Obstructor " + std::to_string(m_Obstructors.size()-1), m_Scene);
		//Engine::Entity ent = Engine::EntityInitializer::GetInstance().EntityInit(Engine::ShaderType::Flat, "Obstructor " + std::to_string(m_Obstructors.size() - 1), m_VA, m_Scene, glm::vec3(0.7, 0.4, 0.2));
		Engine::Entity ent = Engine::EntityInitializer::GetInstance().EntityInit(Engine::ShaderType::Flat, "BeveledCube"/* + std::to_string(m_Obstructors.size() - 1)*/, m_VA, m_Scene, glm::vec3(0.7, 0.4, 0.2));

		//ent.AddComponent<Engine::RendererComponent>();
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
		size_t size = m_Obstructors.size();
		if (size == 0) return;
		Engine::NodeGridSystem::DeleteObstructionSphere(0, size - 1);
		m_Obstructors.erase(--m_Obstructors.end());
	}
};
