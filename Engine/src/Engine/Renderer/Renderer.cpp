#include "epch.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Engine/Scene/Components.h"

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

	void Renderer::Submit(const ShaderType& shaderType, const std::shared_ptr<Shader>& shader,
		const std::shared_ptr<VertexArray>& vertexArray, Entity& entity)
	{
		shader->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ProjectionView", m_SceneData->ProjectionMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewMatrix", m_SceneData->ViewMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", 
			entity.GetComponent<Engine::TransformComponent>().m_Transform);

		switch (shaderType)
		{
			case ShaderType::Flat:
			{
				auto flatOpenGLShader = std::dynamic_pointer_cast<Engine::OpenGLShader>(shader);
				flatOpenGLShader->UploadUniformInt("u_CustomColor", entity.GetComponent<Engine::RendererComponent>().m_bCustomColor);
				flatOpenGLShader->UploadUniformFloat3("u_Color", glm::vec3(entity.GetComponent<Engine::RendererComponent>().m_Color));
				break;
			}
			case ShaderType::Texture:
			{
				entity.GetComponent<TextureComponent>().m_Tex->Bind();
				break;
			}
		}

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

	void Renderer::Submit(PerspectiveCameraController& camController, const ShaderType& shaderType,
		const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, Entity& entity, Entity& light)
	{
		shader->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ProjectionView", m_SceneData->ProjectionMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewMatrix", m_SceneData->ViewMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform",
			entity.GetComponent<Engine::TransformComponent>().m_Transform);

		auto flatOpenGLShader = std::dynamic_pointer_cast<Engine::OpenGLShader>(shader);
		flatOpenGLShader->UploadUniformInt("u_CustomColor", entity.GetComponent<Engine::RendererComponent>().m_bCustomColor);
		flatOpenGLShader->UploadUniformFloat3("u_Color", glm::vec3(entity.GetComponent<Engine::RendererComponent>().m_Color));
		entity.GetComponent<TextureComponent>().m_Tex->Bind();

		auto lightPos = light.GetComponent<Engine::TransformComponent>().m_Transform[3];
		auto tempLight = light.GetComponent<Engine::LightComponent>();

		auto phongOpenGLShader = std::dynamic_pointer_cast<Engine::OpenGLShader>(shader);
		phongOpenGLShader->UploadUniformFloat3("u_LightPosition", glm::vec3{lightPos.x, lightPos.y, lightPos.z});
		phongOpenGLShader->UploadUniformFloat3("u_CameraPosition", camController.GetPos());
		phongOpenGLShader->UploadUniformFloat3("u_LightColor", glm::vec3(tempLight.m_LightColor.x,
			tempLight.m_LightColor.y, tempLight.m_LightColor.z));
		phongOpenGLShader->UploadUniformFloat("u_SpecularStrength", tempLight.m_SpecularStrength);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

	inline glm::vec2 Renderer::GetWindowSize()
	{
		return glm::vec2(m_WindowSize->X, m_WindowSize->Y);
	}
}