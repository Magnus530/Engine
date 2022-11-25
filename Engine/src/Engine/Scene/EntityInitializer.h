/* The video by Portfolio Course on Youtube, was a helpful source when creating this class.
https://www.youtube.com/watch?v=wP3glUDGhi8 */

#pragma once

#include "Engine/AssetInit/ObjLoader.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Renderer/vertex.h"
#include "Engine/Renderer/VertexArray.h"

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
		static Engine::Entity EntityInit(int shapenum, std::shared_ptr<Engine::VertexArray>& vertexarr, std::shared_ptr<Engine::Scene>& Scene);

	protected:
		EntityInitializer() = default;

	private:
		static EntityInitializer* m_Instance;
	};
}