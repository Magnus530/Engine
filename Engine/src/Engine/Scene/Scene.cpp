#include "epch.h"
#include "Scene.h"

#include <glm/glm.hpp>

namespace Engine
{
	static void DoMath(const glm::mat4& transform)
	{}

	// Called when an enitity is created with this component
	static void OnTransformConstruct(entt::registry& registry, entt::entity entity)
	{}

	Scene::Scene()
	{
		// How to create a component
		struct TransformComponent
		{
			glm::mat4 Transform;

			TransformComponent() = default;
			TransformComponent(const TransformComponent&) = default;
			TransformComponent(const glm::mat4& transform)
				: Transform(transform) {}

			// Operator overload for DoMath parameter
			operator glm::mat4& () { return Transform; }
			operator const glm::mat4&() const { return Transform; }
		};

		struct MeshComponent
		{};

		// Example
		TransformComponent transform;
		DoMath(transform);

		// How to create an entity
		entt::entity entity = m_Registry.create();

		// How to add a component to an entity
		m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));

		// Calls function when a specific component is created
		m_Registry.on_construct<TransformComponent>().connect<&OnTransformConstruct>();

		// How to remove component
		m_Registry.remove<TransformComponent>(entity);
		m_Registry.clear();

		//if (m_Registry.has<TransformComponent>(entity))
		//{
		//	TransformComponent& transform = m_Registry.get<TransformComponent>(entity);
		//}

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
			//auto&[transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
			//Renderer::submit(mesh, transform);
		}

	}

	Scene::~Scene()
	{
	}
}