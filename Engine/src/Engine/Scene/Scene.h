#pragma once

#include "entt.hpp"

#include "Engine/Core/Timestep.h"
#include "Engine/Renderer/Texture.h"

namespace Engine
{
	class Entity;
	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());

		void OnUpdate(Timestep ts);

		std::unordered_map<std::string, std::shared_ptr<Engine::Entity>> m_Entities;
		std::unordered_map<std::string, std::shared_ptr<Engine::Texture2D>> m_Textures;

		std::unordered_map<std::string, std::shared_ptr<Engine::TextureCubemap>> m_Skyboxes;

	private:
		entt::registry m_Registry;

		friend class Entity;
	};
}