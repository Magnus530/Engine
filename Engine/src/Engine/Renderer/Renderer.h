#pragma once

#include "RenderCommand.h"
#include "Shader.h"
#include "Camera.h"
#include "Engine/Scene/Entity.h"

namespace Engine
{
	class Renderer
	{
	public:
		static void Init();
		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(Camera& camera);
		static void EndScene();

		//static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4 transform = glm::mat4(1.0f));
		static void Submit(const ShaderType& shaderType, const std::shared_ptr<Shader>& shader,
			const std::shared_ptr<VertexArray>& vertexArray, Entity& entity);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

		inline static glm::vec2 GetWindowSize();
	private:
		struct SceneData
		{
			glm::mat4 ProjectionMatrix;
			glm::mat4 ViewMatrix;
		};

		static SceneData* m_SceneData;

		struct WindowSize
		{
			unsigned int X = 1280;
			unsigned int Y = 720;
		};

		static WindowSize* m_WindowSize;
	};
}