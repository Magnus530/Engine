#include "epch.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Engine/Scene/Components.h"
#include "Engine/Renderer/RenderContext.h"
#include "Engine/Core/ImGuiSystem.h"

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
		std::shared_ptr<Shader> billboardShader = m_ShaderLibrary->Load("assets/shaders/Billboard.glsl");
	}

	void Renderer::Submit(Entity& entity)
	{
		std::shared_ptr<Engine::RenderContext> contextPtr;

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
			case Engine::ShaderType::Billboard:
			{
				Engine::BillboardShaderState* billboardStatePtr = new Engine::BillboardShaderState;
				contextPtr = std::make_shared<Engine::RenderContext>(billboardStatePtr, entity, m_ShaderLibrary, m_SceneData);
				contextPtr->InitShader();
				break;
			}
		}

		entity.GetComponent<RendererComponent>().m_VA->Bind();
		RenderCommand::DrawIndexed(entity.GetComponent<RendererComponent>().m_VA);
	}

#ifdef E_DEBUG 
	void Renderer::DebugShader(const glm::mat4& transform, const glm::vec3& color)
	{
		auto flatShader = std::dynamic_pointer_cast<Engine::OpenGLShader>(m_ShaderLibrary->Get("Flat"));
		flatShader->Bind();
		flatShader->UploadUniformMat4("u_ProjectionView", m_SceneData->ProjectionMatrix);
		flatShader->UploadUniformMat4("u_ViewMatrix", m_SceneData->ViewMatrix);
		flatShader->UploadUniformMat4("u_Transform", transform);

		flatShader->UploadUniformInt("u_CustomColor", 0);
		flatShader->UploadUniformFloat3("u_Color", color);
	}
	// Render call for DebugOnly objects 
	void Renderer::Submit(const std::shared_ptr<VertexArray> va, glm::vec3 location, float scale, glm::vec3 color)
	{
		DebugShader(glm::scale(glm::mat4(1.f), glm::vec3(scale)) * glm::translate(glm::mat4(1.f), location), color);
		va->Bind();
		RenderCommand::DrawIndexed(va);
	}
	void Renderer::Submit(const std::shared_ptr<VertexArray> va, ConvexPolygon& poly, glm::mat4 transform, glm::vec3 color)
	{
		DebugShader(transform, color);
		va->Bind();
		RenderCommand::DrawLineLoop(va);
	}
	void Renderer::SubmitPoint(const std::shared_ptr<VertexArray> va, glm::vec3 location, float size, glm::vec3 color)
	{
		DebugShader(glm::translate(glm::mat4(1.f), location), color);
		va->Bind();
		RenderCommand::DrawPoint(va, size);
	}
#endif
	std::shared_ptr<Engine::Texture2D> Renderer::CreateTexture(const std::string name, const std::string filePath, const std::shared_ptr<Engine::Scene>& scene)
	{
		std::shared_ptr<Engine::Texture2D> tempTexture = Engine::Texture2D::Create(filePath);
		scene->m_Textures.insert({name, tempTexture});

		return tempTexture;
	}

	std::shared_ptr<Engine::OpenGLCubemap> Renderer::CreateSkybox(const std::string name, const std::string cubeArr[], const std::shared_ptr<Engine::Scene>& scene)
	{
		std::shared_ptr<Engine::OpenGLCubemap> tempCubemap = std::make_shared<Engine::OpenGLCubemap>(Engine::OpenGLCubemap::OpenGLCubemap(cubeArr));
		scene->m_Skyboxes.insert({ name, tempCubemap });

		return tempCubemap;
	}

	inline glm::vec2 Renderer::GetWindowSize()
	{
		return glm::vec2(m_WindowSize->X, m_WindowSize->Y);
	}
}