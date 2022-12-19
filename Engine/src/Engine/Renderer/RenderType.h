#pragma once

#include "epch.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Engine/Scene/Components.h"

namespace Engine
{
	class RenderType
	{
	public:
		virtual ~RenderType() = 0;
		virtual void InitShaderUniforms(Engine::Renderer::SceneData* sceneData, Entity& entity, Entity& light,
			PerspectiveCameraController& pCam, std::shared_ptr<Engine::ShaderLibrary> shaderLibrary) = 0;

	protected:
		Engine::Renderer::SceneData* m_SceneData;
	};

	RenderType::~RenderType() {}

	class FlatShader : public RenderType
	{
	public:
		~FlatShader() {};
		virtual void InitShaderUniforms(Engine::Renderer::SceneData* sceneData, Entity& entity, Entity& light,
			PerspectiveCameraController& pCam, std::shared_ptr<Engine::ShaderLibrary> shaderLibrary) override
		{
			std::shared_ptr<Engine::Shader> flatShader = shaderLibrary->Get("Flat");
			std::shared_ptr<Engine::OpenGLShader> flatOpenGLShader = std::dynamic_pointer_cast<Engine::OpenGLShader>(flatShader);

			flatShader->Bind();
			std::dynamic_pointer_cast<OpenGLShader>(flatShader)->UploadUniformMat4("u_ProjectionView", sceneData->ProjectionMatrix);
			std::dynamic_pointer_cast<OpenGLShader>(flatShader)->UploadUniformMat4("u_ViewMatrix", sceneData->ViewMatrix);
			std::dynamic_pointer_cast<OpenGLShader>(flatShader)->UploadUniformMat4("u_Transform",
				entity.GetComponent<Engine::TransformComponent>().m_Transform);

			flatOpenGLShader->UploadUniformInt("u_CustomColor", entity.GetComponent<Engine::RendererComponent>().m_bCustomColor);
			flatOpenGLShader->UploadUniformFloat3("u_Color", glm::vec3(entity.GetComponent<Engine::FlatMaterialComponent>().m_Color));
		}
	};

	class TextureShader : public RenderType
	{
	public:
		~TextureShader() {};
		virtual void InitShaderUniforms(Engine::Renderer::SceneData* sceneData, Entity& entity, Entity& light,
			PerspectiveCameraController& pCam, std::shared_ptr<Engine::ShaderLibrary> shaderLibrary) override
		{
			std::shared_ptr<Engine::Shader> textureShader = shaderLibrary->Get("Texture");
			std::shared_ptr<Engine::OpenGLShader> flatOpenGLShader = std::dynamic_pointer_cast<Engine::OpenGLShader>(textureShader);

			textureShader->Bind();
			std::dynamic_pointer_cast<OpenGLShader>(textureShader)->UploadUniformMat4("u_ProjectionView", sceneData->ProjectionMatrix);
			std::dynamic_pointer_cast<OpenGLShader>(textureShader)->UploadUniformMat4("u_ViewMatrix", sceneData->ViewMatrix);
			std::dynamic_pointer_cast<OpenGLShader>(textureShader)->UploadUniformMat4("u_Transform",
				entity.GetComponent<Engine::TransformComponent>().m_Transform);

			entity.GetComponent<TextureComponent>().m_Tex->Bind();
		}
	};

	class PhongShader : public RenderType
	{
	public:
		~PhongShader() {};
		virtual void InitShaderUniforms(Engine::Renderer::SceneData* sceneData, Entity& entity, Entity& light,
			PerspectiveCameraController& pCam, std::shared_ptr<Engine::ShaderLibrary> shaderLibrary) override
		{
			std::shared_ptr<Engine::Shader> phongShader = shaderLibrary->Get("Phong");
			std::shared_ptr<Engine::OpenGLShader> phongOpenGLShader = std::dynamic_pointer_cast<Engine::OpenGLShader>(phongShader);

			phongShader->Bind();
			std::dynamic_pointer_cast<OpenGLShader>(phongShader)->UploadUniformMat4("u_ProjectionView", sceneData->ProjectionMatrix);
			std::dynamic_pointer_cast<OpenGLShader>(phongShader)->UploadUniformMat4("u_ViewMatrix", sceneData->ViewMatrix);
			std::dynamic_pointer_cast<OpenGLShader>(phongShader)->UploadUniformMat4("u_Transform",
				entity.GetComponent<Engine::TransformComponent>().m_Transform);

			phongOpenGLShader->UploadUniformInt("u_CustomColor", entity.GetComponent<Engine::RendererComponent>().m_bCustomColor);
			phongOpenGLShader->UploadUniformFloat3("u_Color", glm::vec3(entity.GetComponent<Engine::FlatMaterialComponent>().m_Color));
			entity.GetComponent<TextureComponent>().m_Tex->Bind();

			auto tempLight = light.GetComponent<Engine::LightComponent>();

			phongOpenGLShader->UploadUniformFloat3("u_LightPosition", light.GetComponent<Engine::TransformComponent>().GetPosition());
			phongOpenGLShader->UploadUniformFloat3("u_CameraPosition", pCam.GetPos());
			phongOpenGLShader->UploadUniformFloat3("u_LightColor", glm::vec3(tempLight.m_LightColor.x,
				tempLight.m_LightColor.y, tempLight.m_LightColor.z));
			phongOpenGLShader->UploadUniformFloat("u_SpecularStrength", tempLight.m_SpecularStrength);
		}
	};
}