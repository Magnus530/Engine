#pragma once

#include "Engine/Objects/vertex.h"
#include "Engine/AssetLoaders/ObjLoader.h"
#include "Engine/Scene/Entity.h"

#include <string>
#include <vector>

namespace Engine
{
	class EntityInitializer
	{
	public:
		int data = 0;

		EntityInitializer(const EntityInitializer&) = delete;
		EntityInitializer(EntityInitializer&&) = delete;
		EntityInitializer& operator=(const EntityInitializer&) = delete;
		EntityInitializer& operator=(EntityInitializer&&) = delete;

		static EntityInitializer& GetInstance() { return *m_Instance; }

		static Engine::Entity EntityInit(const std::string objname, std::shared_ptr<Engine::VertexArray>& vertexarr, std::shared_ptr<Engine::Scene>& Scene);
		static Engine::Entity EntityInit(std::shared_ptr<Engine::VertexArray>& vertexarr, std::shared_ptr<Engine::Scene>& Scene);

	protected:
		EntityInitializer() = default;

	private:
		static EntityInitializer* m_Instance;
	};
}