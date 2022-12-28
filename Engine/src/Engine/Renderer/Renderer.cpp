#include "epch.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Engine/Scene/Components.h"
#include "Engine/Renderer/RenderContext.h"
#include "Engine/Renderer/CubemapMid.h"

#include "glad/glad.h"
#include "stb_image.h"

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
		std::shared_ptr<Shader> skyboxShader = m_ShaderLibrary->Load("assets/shaders/Skybox.glsl");
	}

	void Renderer::Submit(Entity& entity)
	{
		std::shared_ptr<Engine::RenderContext> contextPtr;

		//Engine::CubemapMid::CubemapRender(m_SceneData);

		switch (entity.GetComponent<Engine::MaterialComponent>().m_ShaderType)
		{
			case Engine::ShaderType::Flat:
			{
				Engine::FlatShaderState* flatStatePtr = new Engine::FlatShaderState;
				contextPtr = std::make_shared<Engine::RenderContext>(flatStatePtr, entity, m_ShaderLibrary, m_SceneData);
				contextPtr->InitShader();
				break;
			}
			case Engine::ShaderType::Texture:
			{
				Engine::TextureShaderState* textureStatePtr = new Engine::TextureShaderState;
				contextPtr = std::make_shared<Engine::RenderContext>(textureStatePtr, entity, m_ShaderLibrary, m_SceneData);
				contextPtr->InitShader();
				break;
			}
			case Engine::ShaderType::Phong:
			{
				Engine::PhongShaderState* phongStatePtr = new Engine::PhongShaderState;
				contextPtr = std::make_shared<Engine::RenderContext>(phongStatePtr, entity, m_ShaderLibrary, m_SceneData);
				contextPtr->InitShader();
				break;
			}
			case Engine::ShaderType::Skybox:
			{
				Engine::SkyboxShaderState* skyboxStatePtr = new Engine::SkyboxShaderState;
				contextPtr = std::make_shared<Engine::RenderContext>(skyboxStatePtr, entity, m_ShaderLibrary, m_SceneData);
				contextPtr->InitShader();
				break;
			}
		}

		entity.GetComponent<RendererComponent>().m_VA->Bind();
		RenderCommand::DrawIndexed(entity.GetComponent<RendererComponent>().m_VA);
	}

	std::shared_ptr<Engine::Texture2D> Renderer::CreateTexture(const std::string name, const std::string filePath, const std::shared_ptr<Engine::Scene>& scene)
	{
		std::shared_ptr<Engine::Texture2D> tempTexture = Engine::Texture2D::Create(filePath);
		scene->m_Textures.insert({name, tempTexture});

		return tempTexture;
	}

	std::shared_ptr<Engine::TextureCubemap> Renderer::CreateSkybox(const std::string name, const std::string cubeArr[], const std::shared_ptr<Engine::Scene>& scene)
	{
		std::shared_ptr<Engine::TextureCubemap> tempSkybox = Engine::TextureCubemap::Create(cubeArr);
		scene->m_Skyboxes.insert({ name, tempSkybox });

		return tempSkybox;
	}

	inline glm::vec2 Renderer::GetWindowSize()
	{
		return glm::vec2(m_WindowSize->X, m_WindowSize->Y);
	}
}