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
	std::shared_ptr<Engine::VertexArray> m_FlagVA;
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
	//bool bRenderPatrolPath{ true };
	bool bShowShaderColor{ true };
	int m_StartNode;
	int m_TargetNode;
	std::vector<int> m_BlockedNodes;
	//std::vector<glm::vec3> m_PatrolPoints;

private: // Collision Visuals
	Engine::ConvexPolygon m_Polygon;
	std::shared_ptr<Engine::VertexArray> m_PolygonVA;


	std::shared_ptr<Engine::VertexArray> m_PointVA;
	std::shared_ptr<Engine::VertexArray> m_LineVA;
	std::vector<glm::vec3> m_RayCastLines;
	std::vector<glm::vec3> m_RayCastCollisionPoints;

private: // Screen RayCasting
	glm::vec2 mousePos;
	//Engine::Entity m_BeveledCube;


public:
	PathfindingLayer()
		: Layer("PathfindingLayer"), m_PCameraController(50.0f, 1280.0f / 720.0f, 0.01f, 1000.0f)
	{
		/* Pathfinding */

		/* Loading obj */
		std::vector<Engine::Vertex> vertices;
		std::vector<uint32_t> indices;

		Engine::ObjLoader::ReadFile("Monkey", vertices, indices);

		m_Scene = std::make_shared<Engine::Scene>();
		m_Entity = Engine::EntityInitializer::GetInstance().EntityInit(Engine::ShaderType::Flat, "Monkey", m_VA, m_Scene, glm::vec3(0.7, 0.4, 0.2));
		m_Entity.AddComponent<Engine::PathfindingComponent>();

		//Engine::NodeGridSystem::CreateGridAtLocation(m_Scene.get(), glm::vec3(0,0,0), glm::vec3((int)20, 0, (int)20), 1);

		// Audio
		m_Audio = std::make_shared<Engine::AudioEngine>();

		/* Set m_Entity location to Pathfinding Node 1 */
		//glm::vec3 startPosition = Engine::NodeGridSystem::GetNodeLocation(m_Scene.get(), 0, 0);
		glm::vec3 startPosition{ 10, 0, 10 };
		Engine::TransformSystem::SetWorldPosition(m_Entity.GetComponent<Engine::TransformComponent>(), startPosition + glm::vec3(0, 0.5f, 0)); //Manually adding extra height
		Engine::TransformSystem::RotateToDirectionVector(m_Entity.GetComponent<Engine::TransformComponent>(), glm::normalize(glm::vec3(-1, 0, 1)));

		m_Entity.GetComponent<Engine::PathfindingComponent>().m_StartNode = 0;
		m_Entity.GetComponent<Engine::PathfindingComponent>().m_Grid = 0;


		// Vertex Array Pathfinding 
			// Obstructions
		InitVertexArray("BeveledCube", m_BeveledCubeVA);	// Bruker denne vertex arrayen flere ganger, s� Initialiserer den for seg selv her
			// Nodes
		InitVertexArray("Plane", m_PlaneVA);
			// Patrol Point
		InitVertexArray("Flag", m_FlagVA);


		std::vector<Engine::Vertex> verts;
		verts.push_back(Engine::Vertex());
		//	Point
		InitVertexArray(m_PointVA, verts);
		// Line
		verts.push_back(Engine::Vertex());
		InitVertexArray(m_LineVA, verts);

		//	Collision Convex Polygon
			// - PLANE
		m_Polygon.m_Locations.push_back({-1, 0, 1});
		m_Polygon.m_Locations.push_back({ 1, 0, 1});
		m_Polygon.m_Locations.push_back({ 1, 0,-1});
		m_Polygon.m_Locations.push_back({-1, 0,-1});
		m_Polygon.m_Normals.push_back({ 0, 0, 1});
		m_Polygon.m_Normals.push_back({ 1, 0, 0});
		m_Polygon.m_Normals.push_back({ 0, 0,-1});
		m_Polygon.m_Normals.push_back({-1, 0, 0});


			// - MAKE CUBE
		//m_Polygon.m_Locations.push_back({-1, 1, 1 });
		//m_Polygon.m_Locations.push_back({ 1, 1, 1 });
		//m_Polygon.m_Locations.push_back({ 1, 1,-1 });
		//m_Polygon.m_Locations.push_back({-1, 1,-1 });
		//m_Polygon.m_Normals.push_back({ 0, 0, 1 });
		//m_Polygon.m_Normals.push_back({ 1, 0, 0 });
		//m_Polygon.m_Normals.push_back({ 0, 0,-1 });
		//m_Polygon.m_Normals.push_back({-1, 0, 0 });
		//
		InitVertexArray(m_PolygonVA, m_Polygon);
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
		if (m_Scene->m_PathfindingNodeGrids.size() != 0)
			Engine::PathfindingSystem::MoveAlongPath(m_Scene.get(), pathfinder, transform, ts);
		Engine::NodeGridSystem::UpdateFalseObstructionNodes(m_Scene.get(), 0);


		/* --------------------- RENDERING ------------------------ */
		Engine::Renderer::Submit(m_Entity);
		
		/*-----------RENDER OBSTRUCTIONS---------------*/
		for (auto& it : m_Obstructors)
			Engine::Renderer::Submit(it);
		
#ifdef E_PATHNODE_DEBUG 
		/* ------- RENDER NODEGRID -------- */
		if (m_Scene->m_PathfindingNodeGrids.size() != 0)
		{
			float scale = 0.1f;
			if (m_Scene->m_PathfindingNodeGrids[0]->bRenderNodegrid)
			{	
				glm::vec4 nodeColor(0.1, 0.5, 0.1, 1);
				glm::vec4 startColor(1, 0, 0, 1);
				glm::vec4 targetColor(0, 0, 1, 1);
				glm::vec4 blockColor(0, 0, 0, 1);

				bool alteredColor{};
				auto grid = Engine::NodeGridSystem::GetGridAtIndex(m_Scene.get(), 0);
				for (size_t i{ 0 }; i < grid->m_NodeLocations->size(); i++)
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
			}

			/* ----- RENDER SPLINE PATH ----- */
			scale /= 2.f;
			for (auto& it : pathfinder.m_SplinePath->m_Controlpoints)
				Engine::Renderer::Submit(m_PlaneVA, glm::vec3(it / scale) + glm::vec3(0, 0.2f, 0), scale, {1,1,1});
		}

		/* ----- RENDER PATROL POINTS ----- */
		if (pathfinder.bRenderPatrolPoints)
			for (const auto& it : pathfinder.m_PatrolPath)
				Engine::Renderer::Submit(m_FlagVA, it, 1.f, { 1, 0.2, 0.3 });


		/* ------ RENDER COLLISION POLYGON --------- */
		Engine::Renderer::Submit(m_PolygonVA, m_Polygon, glm::translate(glm::mat4(1.f), {0, 0, 0}));

		float scale = 0.02f;
		for (const auto& pos : m_RayCastCollisionPoints)
			Engine::Renderer::Submit(m_PlaneVA, glm::vec3(pos / scale) + glm::vec3(0, 0.05f, 0), scale, {1,0,0});
			//Engine::Renderer::SubmitPoint(m_PointVA, pos, 100.f);
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

		//ImGui::Checkbox("Render Nodegrid", &bRenderNodeGrid);
		//ImGui::Separator();
		
		std::shared_ptr<Engine::ImGuiSystem> imGuiPtr = std::make_shared<Engine::ImGuiSystem>();
		imGuiPtr->GuiEntitySettings(m_Scene);
		imGuiPtr->GuiPathfindingGridSettings(m_Scene);
																																// ADD TO GUI
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
			ImGui::PushID((int)i);
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
				Engine::NodeGridSystem::UpdateObstructionSphere(m_Scene.get(), 0, obstruction.m_ID, obstruction.m_radius, transform.GetPosition());
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

		if (e.GetEventType() == Engine::EventType::KeyPressed)
		{
			Engine::KeyPressedEvent& ev = static_cast<Engine::KeyPressedEvent&>(e);
			if (ev.GetKeyCode() == E_KEY_C)
			{
				glm::vec3 ray{};
				Engine::RayCast::FromScreenPosition(ray, mousePos, m_PCameraController.GetCamera().GetProjectionMatrix(), m_PCameraController.GetCamera().GetViewMatrix());
				glm::vec3 pos = m_PCameraController.GetCamera().GetPosition();

				for (int i = 0; i < m_Polygon.m_Locations.size(); i++)
				{
					glm::vec3 Intersection{};
					glm::vec3 planeVector = m_Polygon.m_Locations[i + 1 >= m_Polygon.m_Locations.size() ? 0 : i + 1] - m_Polygon.m_Locations[i];

					//glm::vec3 point{};
					//if (Engine::RayCast::IntersectWithAlignedPlane(point, m_Polygon.m_Normals[i], m_Polygon.m_Locations[i], ray, pos))
						//m_RayCastCollisionPoints.push_back(point);
					
					if (Engine::RayCast::IntersectWithPlane(Intersection, m_Polygon.m_Normals[i], m_Polygon.m_Locations[i], planeVector, ray, pos))
						m_RayCastCollisionPoints.push_back(Intersection);
				}
			}
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
			if (m_Scene->m_PathfindingNodeGrids.size() == 0) return;
			// Start Pathfinding - Find raycasted point on XZ plane - Node closest to location
			glm::vec3 ray{};
			Engine::RayCast::FromScreenPosition(ray, mousePos, m_PCameraController.GetCamera().GetProjectionMatrix(), m_PCameraController.GetCamera().GetViewMatrix());
			glm::vec3 pos = m_PCameraController.GetCamera().GetPosition();
			glm::vec3 Intersection;
			if (Engine::RayCast::IntersectWithWAPlaneXZ(Intersection, ray, pos)) 
			{
				auto& transform = m_Entity.GetComponent<Engine::TransformComponent>();
				m_StartNode = Engine::PathfindingSystem::GetNodeClosestToPosition(m_Scene.get(), 0, transform.GetPosition());
				m_TargetNode = Engine::PathfindingSystem::GetNodeClosestToPosition(m_Scene.get(), 0, Intersection);

				auto& pathfinder = m_Entity.GetComponent<Engine::PathfindingComponent>();
				pathfinder.m_TargetNode = m_TargetNode;
				Engine::PathfindingSystem::FindPath(m_Scene.get(), pathfinder, transform.GetPosition() - glm::vec3(0, 0.5f, 0));
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
	void InitVertexArray(std::shared_ptr<Engine::VertexArray>& vertexarr, Engine::ConvexPolygon& poly)
	{
		std::vector<Engine::Vertex> vertices; 
		std::vector<uint32_t> indices;

		for (int i = 0; i < poly.m_Locations.size(); i++)
		{
			Engine::Vertex vert(poly.m_Locations[i], poly.m_Normals[i]);
			vertices.push_back(vert);
			indices.push_back(i);
		}

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
	void InitVertexArray(std::shared_ptr<Engine::VertexArray>& vertexarr, std::vector<Engine::Vertex> vertices)
	{
		std::vector<uint32_t> indices;
		for (int i = 0; i < vertices.size(); i++)
			indices.push_back(i);

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
		Engine::Entity ent = Engine::EntityInitializer::GetInstance().EntityInit(Engine::ShaderType::Flat, "BeveledCube", m_VA, m_Scene, glm::vec3(0.7, 0.4, 0.2));

		ent.AddComponent<Engine::ObstructionSphereComponent>();
		auto& obs = ent.GetComponent<Engine::ObstructionSphereComponent>();
		obs.m_radius = radius;
		auto& t = ent.GetComponent<Engine::TransformComponent>();
		Engine::TransformSystem::SetWorldPosition(t, pos);
		obs.m_ID = Engine::NodeGridSystem::CreateObstructionSphere(m_Scene.get(), 0, obs.m_radius, t.GetPosition());
		
		m_Obstructors.push_back(ent);
	}
	void DeleteObstructor()
	{
		size_t size = m_Obstructors.size();
		if (size == 0) return;
		Engine::NodeGridSystem::DeleteObstructionSphere(m_Scene.get(), 0, (uint32_t)size - 1);
		m_Obstructors.erase(--m_Obstructors.end());
	}
};
