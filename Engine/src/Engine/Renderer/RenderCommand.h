#pragma once

#include "RendererAPI.h"


namespace Engine
{
	class RenderCommand
	{
	public:
		inline static void Init()
		{
			s_RendererAPI->Init();
		}

		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}

		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}

		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
		{
			s_RendererAPI->DrawIndexed(vertexArray);
		}
#ifdef E_DEBUG
		inline static void DrawPoint(const std::shared_ptr<VertexArray>& vertexArray, float size)
		{
			s_RendererAPI->DrawPoint(vertexArray, size);
		}
		inline static void DrawLine(const std::shared_ptr<VertexArray>& vertexArray)
		{
			s_RendererAPI->DrawLine(vertexArray);
		}
		inline static void DrawLineLoop(const std::shared_ptr<VertexArray>& vertexArray)
		{						   
			s_RendererAPI->DrawLineLoop(vertexArray);
		}
#endif
	private:
		static RendererAPI* s_RendererAPI;
	};
}