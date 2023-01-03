#include "epch.h"
#include "Scene.h"

#include "Components.h"

#include <glm/glm.hpp>

#include "Entity.h"

#include "Engine/Scene/Systems/PathfindingSystem.h"

namespace Engine
{
	static void DoMath(const glm::mat4& transform)
	{}

	// Called when an enitity is created with this component
	static void OnTransformConstruct(entt::registry& registry, entt::entity entity)
	{}

	Scene::Scene()
	{
#if ENTT_EXAMPLE_CODE
		// How to create an entity
		entt::entity entity = m_Registry.create();
		// How to add a component to an entity
		m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));
		// Calls function when a specific component is created
		m_Registry.on_construct<TransformComponent>().connect<&OnTransformConstruct>();
		// How to remove component
		m_Registry.remove<TransformComponent>(entity);
		m_Registry.clear();

		// When rendering all entities with transform component
		auto view = m_Registry.view<TransformComponent>();
		for (auto entity : view)
		{
			TransformComponent& transform = m_Registry.get<TransformComponent>(entity);
		}

		// Lets someone create groups of components and loop through them
		auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);
		for (auto entity : group)
		{
			auto&[transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
			//Renderer::submit(mesh, transform);
		}
#endif
	}

	Scene::~Scene()
	{
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	void Scene::OnUpdate(Timestep ts)
	{
		auto group = m_Registry.group<TransformComponent>(entt::get<RendererComponent>);
		for (auto entity : group)
		{
			auto& [transform, rend] = group.get<TransformComponent, RendererComponent>(entity);
		}
	}

	void Scene::CreateObstruction(float radius, glm::vec3 location)
	{
		auto group = m_Registry.group<TransformComponent>(entt::get<ObstructionSphereComponent>);
		std::string name = "Obstruction" + std::to_string(group.size());

		uint32_t id = Engine::NodeGridSystem::CreateObstructionSphere(this, radius, location);
		Engine::Entity ent = Engine::EntityInitializer::GetInstance().ObstructorEntityInit(Engine::ShaderType::Flat, name, "BeveledCube", radius, id, this);
		m_Obstructions.push_back(name);
	}
	void Scene::UpdateObstructionsToNewGrid()
	{
		auto group = m_Registry.group<TransformComponent>(entt::get<ObstructionSphereComponent>);
		for (auto& it : group)
		{
			auto& [transform, obs] = group.get<TransformComponent, ObstructionSphereComponent>(it);
			obs.m_ID = Engine::NodeGridSystem::CreateObstructionSphere(this, obs.m_radius, transform.GetLocation());
		}
	}
	void Scene::DeleteObstruction()
	{
		if (m_Obstructions.size() == 0) return;

		auto group = m_Registry.group<TransformComponent>(entt::get<ObstructionSphereComponent>);
		m_Registry.destroy(*group.begin());
		m_Entities.erase(*--m_Obstructions.end());
		m_Obstructions.erase(--m_Obstructions.end());
		Engine::NodeGridSystem::DeleteObstructionSphere(this, m_Obstructions.size());
	}

#ifdef E_DEBUG
	//void RenderPathfindingNodeGrid(Scene* scene, PathfindingComponent& pathfinder)
	//{
	//	if (scene->m_PathfindingNodeGrids.size() != 0)
	//	{
	//		
	//		float scale = 0.1f;
	//		if (scene->m_PathfindingNodeGrids[0]->bRenderNodegrid)
	//		{
	//			glm::vec4 nodeColor(0.1, 0.5, 0.1, 1);
	//			glm::vec4 startColor(1, 0, 0, 1);
	//			glm::vec4 targetColor(0, 0, 1, 1);
	//			glm::vec4 blockColor(0, 0, 0, 1);
	//
	//			bool alteredColor{};
	//			auto grid = Engine::NodeGridSystem::GetGridAtIndex(scene.get(), 0);
	//			for (size_t i{ 0 }; i < grid->m_NodeLocations->size(); i++)
	//			{
	//				if (i == pathfinder.m_StartNode) {
	//					Engine::Renderer::Submit(m_PlaneVA, glm::vec3(grid->m_NodeLocations->at(i) / scale), scale, startColor);
	//				}
	//				else if (i == m_TargetNode) {
	//					Engine::Renderer::Submit(m_PlaneVA, glm::vec3(grid->m_NodeLocations->at(i) / scale), scale, targetColor);
	//				}
	//				else if (grid->m_NodeObstructionStatus->at(i)) {
	//					Engine::Renderer::Submit(m_PlaneVA, glm::vec3(grid->m_NodeLocations->at(i) / scale), scale, blockColor);
	//				}
	//				else {
	//					Engine::Renderer::Submit(m_PlaneVA, glm::vec3(grid->m_NodeLocations->at(i) / scale), scale, nodeColor);
	//				}
	//			}
	//		}
	//
	//		/* ----- RENDER SPLINE PATH ----- */
	//		scale /= 2.f;
	//		for (auto& it : pathfinder.m_SplinePath->m_Controlpoints)
	//			Engine::Renderer::Submit(m_PlaneVA, glm::vec3(it / scale) + glm::vec3(0, 0.2f, 0), scale, { 1,1,1 });
	//	}
	//
	//	/* ----- RENDER PATROL POINTS ----- */
	//	if (pathfinder.bRenderPatrolPoints)
	//		for (const auto& it : pathfinder.m_PatrolPath)
	//			Engine::Renderer::Submit(m_FlagVA, it, 1.f, { 1, 0.2, 0.3 });
	//
	//}
#endif
}



