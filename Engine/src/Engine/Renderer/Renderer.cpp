#include "epch.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Engine/Scene/Components.h"
#include "Engine/Renderer/RenderFactory.h"

namespace Engine
{
	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;
	Renderer::WindowSize* Renderer::m_WindowSize = new Renderer::WindowSize();

	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
		m_WindowSize->X = width;
		m_WindowSize->Y = height;
	}

	void Renderer::BeginScene(Camera& camera)
	{
		m_SceneData->ProjectionMatrix = camera.GetProjectionMatrix();
		m_SceneData->ViewMatrix = camera.GetViewMatrix();
	}

	void Renderer::EndScene()
	{}

	void Renderer::RenderInit()
	{
		m_ShaderLibrary = std::make_shared<Engine::ShaderLibrary>();

		std::shared_ptr<Shader> flatShader = m_ShaderLibrary->Load("assets/shaders/Flat.glsl");
		std::shared_ptr<Shader> textureShader = m_ShaderLibrary->Load("assets/shaders/Texture.glsl");
		std::shared_ptr<Shader> phongShader = m_ShaderLibrary->Load("assets/shaders/Phong.glsl");

		std::dynamic_pointer_cast<Engine::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<Engine::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
	}

	void Renderer::Submit(const ShaderType& shaderType, const Entity& entity)
	{
	}

	//void Renderer::Submit(const ShaderType& shaderType, const std::shared_ptr<VertexArray>& vertexArray, Entity& entity, Entity& light, PerspectiveCameraController& pCam)
	//{
	//	Engine::RenderFactory::CreateShaderType(shaderType)->InitShaderUniforms(m_SceneData, entity, light, pCam, m_ShaderLibrary);


	//	vertexArray->Bind();
	//	RenderCommand::DrawIndexed(vertexArray);
	//}

	inline glm::vec2 Renderer::GetWindowSize()
	{
		return glm::vec2(m_WindowSize->X, m_WindowSize->Y);
	}
}