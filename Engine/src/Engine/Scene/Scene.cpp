#include "epch.h"
#include "Scene.h"

#include "Components.h"

#include <glm/glm.hpp>

#include "Entity.h"

#include "Engine/Renderer/VertexArray.h"
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
	void Scene::CreateObstruction(std::shared_ptr<VertexArray> va, float radius, glm::vec3 location)
	{
		auto group = m_Registry.group<TransformComponent>(entt::get<ObstructionSphereComponent>);
		std::string name = "Obstruction" + std::to_string(group.size());

		uint32_t id = Engine::NodeGridSystem::CreateObstructionSphere(this, radius, location);
		//Engine::Entity ent = Engine::EntityInitializer::GetInstance().ObstructorEntityInit(Engine::ShaderType::Flat, name, va, radius, id, this/*, glm::vec3(0.2), std::make_pair("Rock", Engine::Texture2D::Create("assets/textures/Rock.png"))*/);
		Engine::Entity ent = Engine::EntityInitializer::GetInstance().ObstructorEntityInit(Engine::ShaderType::Flat, name, va, radius, id, this);

		//Entity ent = CreateEntity(name);
		//ent.AddComponent<RendererComponent>(va);
		//ent.AddComponent<ObstructionSphereComponent>(radius, id);

		Engine::TransformSystem::SetWorldPosition(ent.GetComponent<TransformComponent>(), location);
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
	void Scene::ClearObstructions()
	{
		//for (int i = 0; i < m_Obstructions.size())
		while (m_Obstructions.size() != 0)
			DeleteObstruction();
	}

	void Scene::CreateSceneObstructions(unsigned int amount, glm::vec3 avoidLocation, std::shared_ptr<VertexArray> va)
	{
		// Init and Lambda functions
		glm::vec3 extent = this->m_PathfindingNodeGrid->m_Extent;
		int Xlength = extent.x;
		int Zlength = extent.z;

		float limit = 3.f;
		std::function<bool(glm::vec3&)> validLocation = [limit, avoidLocation](glm::vec3& loc) {
			if (loc.x < avoidLocation.x+limit && loc.x > avoidLocation.x-limit || loc.z < avoidLocation.z+limit && loc.z > avoidLocation.z-limit)
				return false;
			return true;
		};
		std::function<glm::vec3()> randomLocation = [Xlength, Zlength, extent, &randomLocation, validLocation]() {
			float x = (-extent.x + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (Xlength))));
			float z = (-extent.z + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (Zlength))));
			glm::vec3 loc{ x, 0, z };
			if (!validLocation(loc))
				loc = randomLocation();
			return loc;
		};
		
		// Creating Obstructions at 
		E_INFO("CreateSceneObstructions Time");
		TIMER t1 = TIMENOW();
		srand(time(0));
		std::vector<std::future<void>> asyncs;
		for (int i = 0; i < amount; i++)
		{
			/*asyncs.push_back(*/std::async([this, randomLocation](glm::vec3 ex, std::shared_ptr<VertexArray> va)
				{
					glm::vec3 loc = randomLocation();
					CreateObstruction(va, 1.5f, (loc)+(ex / 2.f));
				}, extent, va);
		}
		TIME_MILLI("CREATION = ", t1);
		
		
		// Getting Obstruction Texture
		TIMER t2 = TIMENOW();
		auto tex = std::make_pair("Rock", Engine::Texture2D::Create("assets/textures/Rock.png"));
		TIME_MILLI("GETTING TEXTURE = ", t2);
		
		// Setting the texture for obstruction objects 
		TIMER t3 = TIMENOW();
		for (const auto& it : m_Obstructions)
			std::async([this, it, tex](){ m_Entities[it]->GetComponent<TextureMaterialComponent>().m_Tex = tex; });
		TIME_MILLI("TIME SET TEXTURE = ", t3);
		TIME_SECOND("TIME TOTAL = ", t1);
	}
}



