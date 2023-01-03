#pragma once

#include "Engine/Renderer/RendererAPI.h"

namespace Engine
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;

#ifdef E_DEBUG
		virtual void DrawPoint(const std::shared_ptr<VertexArray>& vertexArray, float size) override;
		virtual void DrawLine(const std::shared_ptr<VertexArray>& vertexArray) override;
		virtual void DrawLineLoop(const std::shared_ptr<VertexArray>& vertexArray) override;
#endif
	};
}
