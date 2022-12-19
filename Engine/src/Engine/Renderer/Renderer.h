#pragma once

#include "RenderCommand.h"
#include "Shader.h"
#include "Camera.h"
#include "Engine/Renderer/PerspectiveCameraController.h"
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

		static void RenderInit();

		//static void Submit(const ShaderType& shaderType, const std::shared_ptr<VertexArray>& vertexArray, Entity& entity, Entity& light, PerspectiveCameraController& pCam);
		static void Submit(const ShaderType& shaderType, const Entity& entity);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

		inline static glm::vec2 GetWindowSize();

		struct SceneData
		{
			glm::mat4 ProjectionMatrix;
			glm::mat4 ViewMatrix;
		};

		static SceneData* m_SceneData;
	private:

		struct WindowSize
		{
			unsigned int X = 1280;
			unsigned int Y = 720;
		};

		static WindowSize* m_WindowSize;

		inline static std::shared_ptr<Engine::ShaderLibrary> m_ShaderLibrary;
	};
}