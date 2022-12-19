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

	//void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4 transform)
	//{
	//	shader->Bind();
	//	std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ProjectionView", m_SceneData->ProjectionMatrix);
	//	std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewMatrix", m_SceneData->ViewMatrix);
	//	std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);
	//	vertexArray->Bind();
	//	RenderCommand::DrawIndexed(vertexArray);
	//}

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
				entity.GetComponent<RendererComponent>().m_Tex->Bind();
				break;
			}
			case ShaderType::Phong:
				break;

		}

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

	inline glm::vec2 Renderer::GetWindowSize()
	{
		return glm::vec2(m_WindowSize->X, m_WindowSize->Y);
	}
}