#include "epch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Engine
{
	std::shared_ptr<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:     E_CORE_ASSERT(false, "RendererAPI::None has yet to be supported."); return nullptr;
			case RendererAPI::API::OpenGL:   return std::make_shared<OpenGLVertexArray>(OpenGLVertexArray());
		}

		E_CORE_ASSERT(false, "Unknown RendererAPI.");
		return nullptr;
	}
}