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
	bool bShowShaderColor{ true };
	std::vector<int> m_BlockedNodes;

private: // Collision Visuals
	Engine::ConvexPolygon m_Polygon;
	std::shared_ptr<Engine::VertexArray> m_PolygonVA;


	std::shared_ptr<Engine::VertexArray> m_PointVA;
	std::shared_ptr<Engine::VertexArray> m_LineVA;
	std::vector<glm::vec3> m_RayCastLines;
	std::vector<glm::vec3> m_RayCastCollisionPoints;

private: // Screen RayCasting
	glm::vec2 mousePos;


public:
	PathfindingLayer()
		: Layer("PathfindingLayer"), m_PCameraController(50.0f, 1280.0f / 720.0f, 0.01f, 1000.0f)
	{
		m_Scene = std::make_shared<Engine::Scene>();
		Engine::Renderer::RenderInit();
		Engine::TextureList::CreateTextures(m_Scene);

		Engine::NodeGridSystem::CreateGridAtLocation(m_Scene.get(), glm::vec3{0,0,0}, glm::vec3{50, 0, 50}, 1);

		m_Entity = Engine::EntityInitializer::GetInstance().EntityInit(Engine::ShaderType::Flat, "Monkey", m_VA, m_Scene, 0, glm::vec3(0.7, 0.4, 0.2));
		m_Entity.AddComponent<Engine::PathfindingComponent>();

		// Audio
		m_Audio = std::make_shared<Engine::AudioEngine>();

		/* Set m_Entity location to Pathfinding Node 1 */
		glm::vec3 startPosition{ 0, 0, 0 };
		Engine::TransformSystem::SetWorldPosition(m_Entity.GetComponent<Engine::TransformComponent>(), startPosition + glm::vec3(0, 0.5f, 0)); //Manually adding extra height
		Engine::TransformSystem::RotateToDirectionVector(m_Entity.GetComponent<Engine::TransformComponent>(), glm::normalize(glm::vec3(-1, 0, 1)));


		// Vertex Array Pathfinding 
			// Nodes
		Engine::InitVertexArray("HorizontalPlane", m_PlaneVA);
			// Patrol Point
		Engine::InitVertexArray("Flag", m_FlagVA);


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
		if (m_Scene->m_PathfindingNodeGrid.get())
			Engine::PathfindingSystem::MoveAlongPath(m_Scene.get(), pathfinder, transform, ts);
		Engine::NodeGridSystem::UpdateFalseObstructionNodes(m_Scene.get());

		/* --------------------- RENDERING ------------------------ */
		for (auto& it = m_Scene->m_Entities.begin(); it != m_Scene->m_Entities.end(); it++)
			Engine::Renderer::Submit(*(it)->second);
		
#ifdef E_DEBUG
		/* ------- RENDER NODEGRID -------- */
		if (m_Scene->m_PathfindingNodeGrid.get())
		{
			float scale = 0.1f;
			if (m_Scene->m_PathfindingNodeGrid->bRenderNodegrid)
			{	
				glm::vec4 nodeColor(0.1, 0.5, 0.1, 1);
				glm::vec4 startColor(1, 0, 0, 1);
				glm::vec4 targetColor(0, 0, 1, 1);
				glm::vec4 blockColor(0, 0, 0, 1);

				bool alteredColor{};
				for (size_t i{ 0 }; i < m_Scene->m_PathfindingNodeGrid->m_NodeLocations->size(); i++)
				{
					if (i == pathfinder.m_StartNode) {
						Engine::Renderer::Submit(m_PlaneVA, glm::vec3(m_Scene->m_PathfindingNodeGrid->m_NodeLocations->at(i) / scale), scale, startColor);
					}
					else if (i == pathfinder.m_TargetNode) {
						Engine::Renderer::Submit(m_PlaneVA, glm::vec3(m_Scene->m_PathfindingNodeGrid->m_NodeLocations->at(i) / scale), scale, targetColor);
					}
					else if (m_Scene->m_PathfindingNodeGrid->m_NodeObstructionStatus->at(i)) {
						Engine::Renderer::Submit(m_PlaneVA, glm::vec3(m_Scene->m_PathfindingNodeGrid->m_NodeLocations->at(i) / scale), scale, blockColor);
					}
					else {
						Engine::Renderer::Submit(m_PlaneVA, glm::vec3(m_Scene->m_PathfindingNodeGrid->m_NodeLocations->at(i) / scale), scale, nodeColor);
					}
				}
			}

			/* ----- RENDER SPLINE PATH ----- */
			if (pathfinder.bRenderPath)
				for (auto& it : pathfinder.m_SplinePath->m_Controlpoints)
					Engine::Renderer::Submit(m_PlaneVA, glm::vec3(it / scale) + glm::vec3(0, 0.2f, 0), scale, { 1,1,1 });
		}

		/* ----- RENDER PATROL POINTS ----- */
		glm::vec3 flagColor{ 1, 0.2, 0.3 };
		int i{};
		float colorStep = 1.f / (float)pathfinder.m_PatrolPath.size();
		if (pathfinder.bRenderPatrolPoints)
			for (const auto& it : pathfinder.m_PatrolPath)
				Engine::Renderer::Submit(m_FlagVA, it, 1.f, flagColor - (flagColor * (colorStep * ++i)));


		/* ------ RENDER COLLISION POLYGON --------- */
		Engine::Renderer::Submit(m_PolygonVA, m_Polygon, glm::translate(glm::mat4(1.f), {0, 0, 0}));

		float scale = 0.02f;
		for (const auto& pos : m_RayCastCollisionPoints)
			Engine::Renderer::Submit(m_PlaneVA, glm::vec3(pos / scale) + glm::vec3(0, 0.05f, 0), scale, {1,0,0});
#endif

		// End Render Scene
		Engine::Renderer::EndScene();
	}

	//----------------------------------------------------------------IMGUI-------------------------------------------------------------------------------------------------------------------------
	virtual void OnImGuiRender() override
	{
		std::shared_ptr<Engine::ImGuiSystem> imGuiPtr = std::make_shared<Engine::ImGuiSystem>();
		imGuiPtr->GuiEntitySettings(m_Scene);
		imGuiPtr->GuiPathfindingGridSettings(m_Scene);
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
			if (!m_Scene->m_PathfindingNodeGrid.get()) return;
			// Start Pathfinding - Find raycasted point on XZ plane - Node closest to location
			glm::vec3 ray{};
			Engine::RayCast::FromScreenPosition(ray, mousePos, m_PCameraController.GetCamera().GetProjectionMatrix(), m_PCameraController.GetCamera().GetViewMatrix());
			glm::vec3 pos = m_PCameraController.GetCamera().GetPosition();
			glm::vec3 Intersection;
			if (Engine::RayCast::IntersectWithWAPlaneXZ(Intersection, ray, pos)) 
			{
				auto& transform = m_Entity.GetComponent<Engine::TransformComponent>();
				auto& pathfinder = m_Entity.GetComponent<Engine::PathfindingComponent>();
				pathfinder.m_StartNode = Engine::PathfindingSystem::GetNodeClosestToPosition(m_Scene.get(), transform.GetLocation());
				pathfinder.m_TargetNode = Engine::PathfindingSystem::GetNodeClosestToPosition(m_Scene.get(), Intersection);

				Engine::PathfindingSystem::FindPath(m_Scene.get(), pathfinder, transform.GetLocation() - glm::vec3(0, 0.5f, 0));
			}
		}

	}



	//----------------------------------------------------------------Smaller Specific Functions-------------------------------------------------------------------------------------------------------------------------
	//void InitVertexArray(std::string objname, std::shared_ptr<Engine::VertexArray>& vertexarr)
	//{
	//	std::vector<Engine::Vertex> vertices;
	//	std::vector<uint32_t> indices;

	//	Engine::ObjLoader::ReadFile(objname, vertices, indices);
	//	vertexarr.reset(Engine::VertexArray::Create());
	//	std::shared_ptr<Engine::VertexBuffer> ObjVB;
	//	ObjVB.reset(Engine::VertexBuffer::Create(vertices.data(), (uint32_t)vertices.size() * sizeof(Engine::Vertex))); // OpenGLVertexBuffer*	// for en vector av floats
	//	ObjVB->SetLayout
	//	({
	//		{ Engine::ShaderDataType::Float3, "a_Position" },
	//		{ Engine::ShaderDataType::Float3, "a_Normal" },
	//		{ Engine::ShaderDataType::Float2, "a_TexCoord" }
	//		});
	//	vertexarr->AddVertexBuffer(ObjVB);

	//	std::shared_ptr<Engine::IndexBuffer> ObjIB;
	//	ObjIB.reset(Engine::IndexBuffer::Create(indices)); // OpenGLIndexBuffer*
	//	vertexarr->SetIndexBuffer(ObjIB);
	//}
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

	//void CreateObstructor() {
	//	CreateObstructor(glm::vec3(0.f));
	//}
	//void CreateObstructor(glm::vec3 pos, float radius = 2.f)
	//{
	//	int id = Engine::NodeGridSystem::CreateObstructionSphere(m_Scene.get(), radius, pos);
	//	Engine::Entity ent = Engine::EntityInitializer::GetInstance().ObstructorEntityInit(Engine::ShaderType::Flat, "Obstructor" + std::to_string(id), "BeveledCube", radius, id, m_Scene.get());
	//	m_Obstructors.push_back(ent);
	//}
	//void DeleteObstructor()
	//{
	//	size_t size = m_Obstructors.size();
	//	if (size == 0) return;
	//	Engine::NodeGridSystem::DeleteObstructionSphere(m_Scene.get(), (uint32_t)size - 1);

	//	m_Obstructors.erase(--m_Obstructors.end());
	//}
};
