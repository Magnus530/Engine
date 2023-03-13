/* The game engine series by The Cherno on Youtube, was a helpful source when creating this engine.
https://www.youtube.com/watch?v=JxIZbV_XjAs&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT&index=1 */

#include "Engine.h"
#include "Engine/Core/EntryPoint.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Engine/AssetInit/ObjLoader.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "Platform/OpenGL/OpenGLCubemap.h"

#include <unordered_map>

// Layers
#include "PathfindingLayer.h"
#include "TransformExampleLayer.h"

class MainLayer : public Engine::Layer
{
public:
	MainLayer()
		: Layer("Main"), m_OCameraController(1280.0f / 720.0f, true), m_PCameraController(50.0f, 1280.0f / 720.0f, 0.01f, 1000.0f)
	{
		m_ActiveScene = std::make_shared<Engine::Scene>();
		m_Audio = std::make_shared<Engine::AudioEngine>();
		Engine::Renderer::RenderInit();
		Engine::TextureList::CreateTextures(m_ActiveScene);

		Engine::NodeGridSystem::CreateGridAtLocation(m_ActiveScene.get(), glm::vec3{ 0,0,0 }, glm::vec3{ 50, 0, 50 }, 1);

		//Create entities here
		m_SkyboxEntity = Engine::EntityInitializer::GetInstance().EntityInit("Skybox", m_SkyboxVA, m_ActiveScene, *m_ActiveScene->m_Skyboxes.find("Forest"));

		m_ParticleSourceEntity = Engine::EntityInitializer::GetInstance().EntityInit(Engine::ShaderType::Particle, "Source", m_SphereVA, m_ActiveScene, 0, glm::vec3{ 1.0f, 1.0f, 1.0f }, *m_ActiveScene->m_Textures.find("Leaf"));
		Engine::TransformSystem::SetWorldPosition(m_ParticleSourceEntity.GetComponent<Engine::TransformComponent>(), glm::vec3{ 0.0f, 4.0f, 1.0f });
		Engine::TransformSystem::SetScale(m_ParticleSourceEntity.GetComponent<Engine::TransformComponent>(), glm::vec3{ 1.0f, 1.0f, 1.0f });
		m_ParticleSourceEntity.AddComponent<Engine::ParticleManagerComponent>(20, glm::vec4(3.0f));

		m_ParticleSourceEntity = Engine::EntityInitializer::GetInstance().EntityInit(Engine::ShaderType::Particle, "Source", m_SphereVA, m_ActiveScene, 0, glm::vec3{ 1.0f, 1.0f, 1.0f }, *m_ActiveScene->m_Textures.find("Leaf"));
		Engine::TransformSystem::SetWorldPosition(m_ParticleSourceEntity.GetComponent<Engine::TransformComponent>(), glm::vec3{ 6.0f, 3.0f, 1.0f });
		Engine::TransformSystem::SetScale(m_ParticleSourceEntity.GetComponent<Engine::TransformComponent>(), glm::vec3{ 1.0f, 1.0f, 1.0f });
		m_ParticleSourceEntity.AddComponent<Engine::ParticleManagerComponent>(20, glm::vec4(3.0f));

		m_LightEntity = Engine::EntityInitializer::GetInstance().EntityInit(Engine::ShaderType::Flat, "Sphere", m_SphereVA, m_ActiveScene, 0, glm::vec3{ 1.0f, 1.0f, 0.0f });
		m_LightEntity.AddComponent<Engine::LightComponent>(float{ 0.2f }, float { 2.f });
		Engine::TransformSystem::SetWorldPosition(m_LightEntity.GetComponent<Engine::TransformComponent>(), glm::vec3{ 0.0f, 10.0f, 1.0f });
		Engine::TransformSystem::SetScale(m_LightEntity.GetComponent<Engine::TransformComponent>(), glm::vec3{ 0.5f, 0.5f, 0.5f });

		m_Player = Engine::EntityInitializer::GetInstance().EntityInit(Engine::ShaderType::Texture, "Monkey", m_PlayerVA, m_ActiveScene, 0, glm::vec3(0.7, 0.4, 0.2), *m_ActiveScene->m_Textures.find("Chess"));
		m_Player.AddComponent<Engine::PathfindingComponent>();
		Engine::TransformSystem::SetWorldPosition(m_Player.GetComponent<Engine::TransformComponent>(), glm::vec3{ 0.0f, 2.0f, 0.0f });

		Engine::EntityInitializer::GetInstance().CreateForest("Plane", 70, glm::vec3{4.f,0.f,0.f}, 50, m_ActiveScene);

		//m_Terrain = Engine::EntityInitializer::GetInstance().EntityInit(Engine::ShaderType::Terrain, "Terrain", m_PlaneVA, m_ActiveScene, 0, glm::vec3{ 1.0f, 1.0f, 1.0f });
		//Engine::TransformSystem::SetScale(m_Terrain.GetComponent<Engine::TransformComponent>(), glm::vec3{ 1.0f, 1.0f, 1.0f });
		Engine::EntityInitializer::GetInstance().EntityInit(Engine::ShaderType::Phong, "Terr1", m_PlaneVA, m_ActiveScene, 0, glm::vec3{ 1.0f, 1.0f, 1.0f }, *m_ActiveScene->m_Textures.find("Base"));

		Engine::InitVertexArray("Flag", m_FlagVA);
		Engine::InitVertexArray("HorizontalPlane", m_FlatPlaneVA);

		m_ParticleManager = new particles::BasicParticleManager(20, glm::vec4{0,10,0,0});
	}

	void OnUpdate(Engine::Timestep ts) override
	{
		m_PCameraController.OnUpdate(ts);
		m_Audio->update(ts);
		Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Engine::RenderCommand::Clear();

		Engine::Renderer::BeginScene(m_PCameraController.GetCamera());

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		for (auto& it = m_ActiveScene->m_Entities.begin(); it != m_ActiveScene->m_Entities.end(); it++)
		{
			Engine::Renderer::Submit(*(it)->second);

			if ((it)->second->HasComponent<Engine::PhongMaterialComponent>())
			{
				Engine::LightSystem::UpdateLight((it)->second->GetComponent<Engine::PhongMaterialComponent>(), m_LightEntity, m_PCameraController);
			}

			if ((it)->second->HasComponent<Engine::BillboardMaterialComponent>())
			{
				Engine::BillboardSystem::UpdateBillboard(*(it)->second, m_PCameraController);
			}

			if ((it)->second->HasComponent<Engine::ParticleMaterialComponent>())
			{
				//m_ParticleManager->update(static_cast<float>(ts));
				if ((it)->second->HasComponent<Engine::ParticleManagerComponent>())
				{
					Engine::ParticleSystem::UpdateParticle((it)->second->GetComponent<Engine::ParticleManagerComponent>(), static_cast<float>(ts));
				}
			}
		}
		Pathfinding_Update(ts);
#ifdef E_DEBUG
		Pathfinding_RenderNodeGrid();
		/* ----- RENDER PATROL POINTS ----- */
		if (m_Player)
		{
			auto& pathfinder = m_Player.GetComponent<Engine::PathfindingComponent>();
			glm::vec3 flagColor{ 1, 0.2, 0.3 };
			int i{};
			float colorStep = 1.f / (float)pathfinder.m_PatrolPath.size();
			if (pathfinder.bRenderPatrolPoints)
				for (const auto& it : pathfinder.m_PatrolPath)
					Engine::Renderer::Submit(m_FlagVA, it, 1.f, flagColor - (flagColor * (colorStep * ++i)));
		}
#endif

		Engine::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		std::shared_ptr<Engine::ImGuiSystem> imGuiPtr = std::make_shared<Engine::ImGuiSystem>();
		imGuiPtr->GuiEntitySettings(m_ActiveScene);
		imGuiPtr->GuiPathfindingGridSettings(m_ActiveScene);
		imGuiPtr->GuiAudioSettings(m_Audio);
	}

	void OnEvent(Engine::Event& e) override
	{
		m_PCameraController.OnEvent(e);
		Pathfinding_MouseEvent(e);
	}

	void Pathfinding_Update(float ts)
	{
		// Moving Entities through node grid
		auto& transform = m_Player.GetComponent<Engine::TransformComponent>();
		auto& pathfinder = m_Player.GetComponent<Engine::PathfindingComponent>();
		if (m_ActiveScene->m_PathfindingNodeGrid.get())
			Engine::PathfindingSystem::MoveAlongPath(m_ActiveScene.get(), pathfinder, transform, ts);
		Engine::NodeGridSystem::UpdateFalseObstructionNodes(m_ActiveScene.get());

	}
	void Pathfinding_MouseEvent(Engine::Event& e)
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
				m_Player.GetComponent<Engine::PathfindingComponent>().m_PatrolPath.push_back(Intersection);
			}
		}
		else if (e.GetEventType() == Engine::EventType::MouseButtonPressed && Engine::Input::IsMouseButtonPressed(E_MOUSE_BUTTON_LEFT))
		{
			if (!m_ActiveScene->m_PathfindingNodeGrid.get()) return;
			// Start Pathfinding - Find raycasted point on XZ plane - Node closest to location
			glm::vec3 ray{};
			Engine::RayCast::FromScreenPosition(ray, mousePos, m_PCameraController.GetCamera().GetProjectionMatrix(), m_PCameraController.GetCamera().GetViewMatrix());
			glm::vec3 pos = m_PCameraController.GetCamera().GetPosition();
			glm::vec3 Intersection;
			if (Engine::RayCast::IntersectWithWAPlaneXZ(Intersection, ray, pos))
			{
				auto& transform = m_Player.GetComponent<Engine::TransformComponent>();
				auto& pathfinder = m_Player.GetComponent<Engine::PathfindingComponent>();
				pathfinder.m_StartNode = Engine::PathfindingSystem::GetNodeClosestToPosition(m_ActiveScene.get(), transform.GetLocation());
				pathfinder.m_TargetNode = Engine::PathfindingSystem::GetNodeClosestToPosition(m_ActiveScene.get(), Intersection);

				Engine::PathfindingSystem::FindPath(m_ActiveScene.get(), pathfinder, transform.GetLocation() - glm::vec3(0, 0.5f, 0));
			}
		}
	}
#ifdef E_DEBUG
	void Pathfinding_RenderNodeGrid()
	{
		if (m_ActiveScene->m_PathfindingNodeGrid.get())
		{
			auto& pathfinder = m_Player.GetComponent<Engine::PathfindingComponent>();
			float scale = 0.1f;
			if (m_ActiveScene->m_PathfindingNodeGrid->bRenderNodegrid)
			{
				glm::vec4 nodeColor(0.1, 0.5, 0.1, 1);
				glm::vec4 startColor(1, 0, 0, 1);
				glm::vec4 targetColor(0, 0, 1, 1);
				glm::vec4 blockColor(0, 0, 0, 1);

				bool alteredColor{};
				Engine::Renderer::BindDebugShader();
				for (size_t i{ 0 }; i < m_ActiveScene->m_PathfindingNodeGrid->m_NodeLocations->size(); i++)
				{
					if (i == pathfinder.m_StartNode) {
						Engine::Renderer::Submit(m_FlatPlaneVA, glm::vec3(m_ActiveScene->m_PathfindingNodeGrid->m_NodeLocations->at(i) / scale), scale, startColor);
					}
					else if (i == pathfinder.m_TargetNode) {
						Engine::Renderer::Submit(m_FlatPlaneVA, glm::vec3(m_ActiveScene->m_PathfindingNodeGrid->m_NodeLocations->at(i) / scale), scale, targetColor);
					}
					else if (m_ActiveScene->m_PathfindingNodeGrid->m_NodeObstructionStatus->at(i)) {
						Engine::Renderer::Submit(m_FlatPlaneVA, glm::vec3(m_ActiveScene->m_PathfindingNodeGrid->m_NodeLocations->at(i) / scale), scale, blockColor);
					}
					else {
						Engine::Renderer::Submit(m_FlatPlaneVA, glm::vec3(m_ActiveScene->m_PathfindingNodeGrid->m_NodeLocations->at(i) / scale), scale, nodeColor);
					}
				}
			}
			/* ----- RENDER SPLINE PATH ----- */
			scale /= 2.f;
			if (pathfinder.bRenderPath)
				for (auto& it : pathfinder.m_SplinePath->m_Controlpoints)
					Engine::Renderer::Submit(m_FlatPlaneVA, glm::vec3(it / scale) + glm::vec3(0, 0.2f, 0), scale, glm::vec3{ 1,1,1 });
		}
	}
#endif

private:
	std::shared_ptr<Engine::Scene> m_ActiveScene;
	std::shared_ptr<Engine::AudioEngine> m_Audio;

	Engine::PerspectiveCameraController m_PCameraController;
	Engine::OrthographicCameraController m_OCameraController;

	std::shared_ptr<Engine::VertexArray> m_PlaneVA, m_CubeVA, m_SphereVA, m_SkyboxVA, m_PlayerVA, m_RockVA, m_FlagVA, m_FlatPlaneVA;

	// Entities
	Engine::Entity m_SoundEntity;
	Engine::Entity m_CubeEntity;
	Engine::Entity m_PlaneEntity;
	Engine::Entity m_LightEntity;
	Engine::Entity m_SkyboxEntity;
	Engine::Entity m_TempEntity;
	Engine::Entity m_Player;
	Engine::Entity m_ParticleSourceEntity;
	Engine::Entity m_Terrain;

	particles::BasicParticleManager* m_ParticleManager;

	// Mouse position on screen
	glm::vec2 mousePos;
};

class Sandbox : public Engine::Application
{
public:
	Sandbox()
	{
		PushLayer(new MainLayer());
		PushLayer(new PathfindingLayer());
		PushLayer(new TransformExampleLayer());
		
		SetCurrentLayer(0);
		SetGuiLayerNames();
	}

	~Sandbox()
	{}

private:
	std::shared_ptr<Engine::Layer> m_CurrentLayer;
};

Engine::Application* Engine::CreateApplication()
{
	return new Sandbox();
}
