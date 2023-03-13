/* The video by Portfolio Course on Youtube, was a helpful source when creating this class.
https://www.youtube.com/watch?v=wP3glUDGhi8 */

#pragma once

#include "Engine/AssetInit/ObjLoader.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Renderer/vertex.h"
#include "Engine/Renderer/VertexArray.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/TextureList.h"

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

		static Engine::Entity EntityInit(const Engine::ShaderType& shaderType, std::string objname, std::shared_ptr<Engine::VertexArray>& vertexarr, 
			std::shared_ptr<Engine::Scene>& scene, const bool& isBBoard = 0, const glm::vec3& color = { 1.f, 1.f, 1.f }, 
			std::pair<std::string, std::shared_ptr<Engine::Texture2D>> tex = std::make_pair("White", Engine::Texture2D::Create("assets/textures/white.png")));
		// For Skybox
		static Engine::Entity EntityInit(const std::string objname, std::shared_ptr<Engine::VertexArray>& va, std::shared_ptr<Engine::Scene>& scene,
			std::pair<std::string, std::shared_ptr<Engine::OpenGLCubemap>> cubetex);

		static Engine::Entity EntityInit(int shapenum, std::shared_ptr<Engine::VertexArray>& vertexarr, std::shared_ptr<Engine::Scene>& Scene);

		static void MaterialInit(const Engine::ShaderType& shaderType, Engine::Entity& entity, const bool& isBBoard = 0, const glm::vec3& color = { 1.f, 1.f, 1.f },
			std::pair<std::string, std::shared_ptr<Engine::Texture2D>> tex = std::make_pair("White", Engine::Texture2D::Create("assets/textures/white.png")));
		// For Skybox
		static void MaterialInit(Engine::Entity& entity, std::pair<std::string, std::shared_ptr<Engine::OpenGLCubemap>> cubetex);

		static Entity ObstructorEntityInit(const ShaderType shaderType, const std::string objname, std::shared_ptr<VertexArray> va, float radius,
			uint32_t id, Scene* scene, glm::vec3 color, 
			std::pair<std::string, std::shared_ptr<Engine::Texture2D>> tex = std::make_pair("White", Engine::Texture2D::Create("assets/textures/white.png")));
		static Entity ObstructorEntityInit(const ShaderType shaderType, const std::string objname, std::shared_ptr<VertexArray> va, float radius,
			uint32_t id, Scene* scene);

		static void CreateForest(std::string objname, const float& trees, const glm::vec3& forestCenter, const float& extent, std::shared_ptr<Engine::Scene>& scene);

		//static std::shared_ptr<VertexArray> VertexArrayInit(const std::string obj);
	protected:
		EntityInitializer() = default;

	private:
		static EntityInitializer* m_Instance;

	};
}