/* The video by Portfolio Course on Youtube, was a helpful source when creating this class.
https://www.youtube.com/watch?v=wP3glUDGhi8 */

#pragma once

#include "Engine/AssetInit/ObjLoader.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Renderer/vertex.h"
#include "Engine/Renderer/VertexArray.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Texture.h"

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

		static Engine::Entity EntityInit(const std::string objname, std::shared_ptr<Engine::Scene>& Scene);
		static Engine::Entity EntityInit(const Engine::ShaderType& shaderType, const std::string objname, std::shared_ptr<Engine::VertexArray>& vertexarr, 
			std::shared_ptr<Engine::Scene>& Scene, const glm::vec3& color = { 1.f, 1.f, 1.f }, std::shared_ptr<Engine::Texture2D>& tex = Engine::Texture2D::Create("assets/textures/white.png"));
		static Engine::Entity EntityInit(int shapenum, std::shared_ptr<Engine::VertexArray>& vertexarr, std::shared_ptr<Engine::Scene>& Scene);

		static void MaterialInit(const Engine::ShaderType& shaderType, Engine::Entity& entity, const glm::vec3& color = { 1.f, 1.f, 1.f }, 
			std::shared_ptr<Engine::Texture2D>& tex = Engine::Texture2D::Create("assets/textures/white.png"));

	protected:
		EntityInitializer() = default;

	private:
		static EntityInitializer* m_Instance;
	};
}