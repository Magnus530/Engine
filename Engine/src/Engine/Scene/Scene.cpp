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

	// Pathfinding
	void Scene::CreateObstruction(float radius, glm::vec3 location)
	{
		auto group = m_Registry.group<TransformComponent>(entt::get<ObstructionSphereComponent>);
		std::string name = "Obstruction" + std::to_string(group.size());

		uint32_t id = Engine::NodeGridSystem::CreateObstructionSphere(this, radius, location);
		Engine::Entity ent = Engine::EntityInitializer::GetInstance().ObstructorEntityInit(Engine::ShaderType::Texture, name, "Rock", radius, id, this, glm::vec3(0.2), std::make_pair("Rock", Engine::Texture2D::Create("assets/textures/Rock.png")));
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
}



