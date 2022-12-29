#pragma once

#include <memory>
#include "Engine/Scene/Entity.h"
#include "Engine/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Engine/Scene/Components.h"

#include "glad/glad.h"


namespace Engine
{
	class RenderContext;

	class RenderState
	{
	public:
		virtual ~RenderState() {}

		void SetRenderContext(Engine::RenderContext* context)
		{
			this->contextPtr = context;
		}

		virtual void InitShader(Entity& entity, std::shared_ptr<Engine::ShaderLibrary> shaderLibrary, Engine::Renderer::SceneData* sceneData) = 0;

	protected:
		Engine::RenderContext* contextPtr = nullptr;
	};

	class FlatShaderState : public RenderState
	{
	public:
		void InitShader(Entity& entity, std::shared_ptr<Engine::ShaderLibrary> shaderLibrary, Engine::Renderer::SceneData* sceneData) override
		{
			std::shared_ptr<Engine::Shader> flatShader = shaderLibrary->Get("Flat");
			std::shared_ptr<Engine::OpenGLShader> flatOpenGLShader = std::dynamic_pointer_cast<Engine::OpenGLShader>(flatShader);

			flatShader->Bind();
			flatOpenGLShader->UploadUniformMat4("u_ProjectionView", sceneData->ProjectionMatrix);
			flatOpenGLShader->UploadUniformMat4("u_ViewMatrix", sceneData->ViewMatrix);
			flatOpenGLShader->UploadUniformMat4("u_Transform", entity.GetComponent<Engine::TransformComponent>().m_Transform);

			flatOpenGLShader->UploadUniformInt("u_CustomColor", entity.GetComponent<Engine::RendererComponent>().m_bCustomColor);
			flatOpenGLShader->UploadUniformFloat3("u_Color", glm::vec3(entity.GetComponent<Engine::FlatMaterialComponent>().m_Color));
		}
	};

	class TextureShaderState : public RenderState
	{
	public:
		void InitShader(Entity& entity, std::shared_ptr<Engine::ShaderLibrary> shaderLibrary, Engine::Renderer::SceneData* sceneData) override
		{
			std::shared_ptr<Engine::Shader> textureShader = shaderLibrary->Get("Texture");
			std::shared_ptr<Engine::OpenGLShader> textureOpenGLShader = std::dynamic_pointer_cast<Engine::OpenGLShader>(textureShader);

			textureShader->Bind();
			textureOpenGLShader->UploadUniformMat4("u_ProjectionView", sceneData->ProjectionMatrix);
			textureOpenGLShader->UploadUniformMat4("u_ViewMatrix", sceneData->ViewMatrix);
			textureOpenGLShader->UploadUniformMat4("u_Transform", entity.GetComponent<Engine::TransformComponent>().m_Transform);

			entity.GetComponent<TextureMaterialComponent>().m_Tex.second->Bind();
		}
	};

	class PhongShaderState : public RenderState
	{
	public:
		void InitShader(Entity& entity, std::shared_ptr<Engine::ShaderLibrary> shaderLibrary, Engine::Renderer::SceneData* sceneData) override
		{
			std::shared_ptr<Engine::Shader> phongShader = shaderLibrary->Get("Phong");
			std::shared_ptr<Engine::OpenGLShader> phongOpenGLShader = std::dynamic_pointer_cast<Engine::OpenGLShader>(phongShader);

			phongShader->Bind();
			phongOpenGLShader->UploadUniformMat4("u_ProjectionView", sceneData->ProjectionMatrix);
			phongOpenGLShader->UploadUniformMat4("u_ViewMatrix", sceneData->ViewMatrix);
			phongOpenGLShader->UploadUniformMat4("u_Transform", entity.GetComponent<Engine::TransformComponent>().m_Transform);

			phongOpenGLShader->UploadUniformInt("u_CustomColor", entity.GetComponent<Engine::RendererComponent>().m_bCustomColor);
			phongOpenGLShader->UploadUniformFloat3("u_Color", glm::vec3(entity.GetComponent<Engine::PhongMaterialComponent>().m_Color));
			entity.GetComponent<PhongMaterialComponent>().m_Tex.second->Bind();

			phongOpenGLShader->UploadUniformFloat("u_AmbientStrength", entity.GetComponent<Engine::PhongMaterialComponent>().m_AmbientStrength);
			phongOpenGLShader->UploadUniformFloat3("u_AmbientColor", entity.GetComponent<Engine::PhongMaterialComponent>().m_AmbientColor);
			phongOpenGLShader->UploadUniformFloat3("u_LightPosition", entity.GetComponent<Engine::PhongMaterialComponent>().m_LightPosition);
			phongOpenGLShader->UploadUniformFloat3("u_CameraPosition", entity.GetComponent<Engine::PhongMaterialComponent>().m_PCamPosition);
			phongOpenGLShader->UploadUniformFloat3("u_LightColor", entity.GetComponent<Engine::PhongMaterialComponent>().m_LightColor);
			phongOpenGLShader->UploadUniformFloat("u_SpecularStrength", entity.GetComponent<Engine::PhongMaterialComponent>().m_SpecularStrength);
		}
	};

	class SkyboxShaderState : public RenderState
	{
	public:
		void InitShader(Entity& entity, std::shared_ptr<Engine::ShaderLibrary> shaderLibrary, Engine::Renderer::SceneData* sceneData) override
		{
			std::shared_ptr<Engine::Shader> skyboxShader = shaderLibrary->Get("Skybox");
			std::shared_ptr<Engine::OpenGLShader> skyboxOpenGLShader = std::dynamic_pointer_cast<Engine::OpenGLShader>(skyboxShader);

			skyboxShader->Bind();
			skyboxOpenGLShader->UploadUniformMat4("u_ProjectionView", sceneData->ProjectionMatrix);
			skyboxOpenGLShader->UploadUniformMat4("u_ViewMatrix", glm::mat4(glm::mat3(sceneData->ViewMatrix)));
			skyboxOpenGLShader->UploadUniformMat4("u_Transform", entity.GetComponent<Engine::TransformComponent>().m_Transform);

			glDepthMask(GL_TRUE);
			skyboxOpenGLShader->UploadUniformInt("skybox", 0);
			glDepthFunc(GL_LEQUAL);
			glBindTexture(GL_TEXTURE_CUBE_MAP, entity.GetComponent<SkyboxMaterialComponent>().m_CubemapTexture);
		}
	};
}