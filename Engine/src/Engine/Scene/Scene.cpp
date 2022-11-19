#include "epch.h"
#include "Scene.h"

#include "Components.h"

#include <glm/glm.hpp>

#include "Entity.h"

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
}



