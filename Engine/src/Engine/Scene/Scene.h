#pragma once

#include "entt.hpp"

#include "Engine/Core/Timestep.h"
#include "Engine/Renderer/Texture.h"

#include <glm/glm.hpp>

namespace Engine
{
	class Entity;
	class OpenGLCubemap;
	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());

		void OnUpdate(Timestep ts);

		std::unordered_map<std::string, std::shared_ptr<Engine::Entity>> m_Entities;
		std::unordered_map<std::string, std::shared_ptr<Engine::Texture2D>> m_Textures;

		std::unordered_map<std::string, std::shared_ptr<Engine::OpenGLCubemap>> m_Skyboxes;

		// Pathfinding
		std::shared_ptr<struct NodeGrid> m_PathfindingNodeGrid;
		std::vector<std::string> m_Obstructions;
		void CreateObstruction(std::shared_ptr<class VertexArray> va, float radius = 2.f, glm::vec3 location = glm::vec3(0.f));
		void UpdateObstructionsToNewGrid();
		void DeleteObstruction();
		void ClearObstructions();
		void CreateSceneObstructions(unsigned int amount, glm::vec3 avoidLocation, std::shared_ptr<class VertexArray> va);

	private:
		entt::registry m_Registry;

		friend class Entity;

	};
}