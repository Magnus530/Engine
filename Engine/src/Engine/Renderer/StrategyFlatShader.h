#pragma once

#include "RenderStrategy.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Engine
{
	class StrategyFlatShader : public RenderStrategy
	{
	public:
		void ShaderSelect() const override
		{

			flatShader->Bind();
			std::dynamic_pointer_cast<OpenGLShader>(flatShader)->UploadUniformMat4("u_ProjectionView", m_SceneData->ProjectionMatrix);
			std::dynamic_pointer_cast<OpenGLShader>(flatShader)->UploadUniformMat4("u_ViewMatrix", m_SceneData->ViewMatrix);
			std::dynamic_pointer_cast<OpenGLShader>(flatShader)->UploadUniformMat4("u_Transform", transform);

			vertexArray->Bind();
			RenderCommand::DrawIndexed(vertexArray);
			flatShader->Bind();
		}
	private:
		std::shared_ptr<Engine::ShaderLibrary> m_ShaderLibrary;
	};
}