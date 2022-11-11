#include "epch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Engine
{
	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None:     E_CORE_ASSERT(false, "RendererAPI::None has yet to be supported."); return nullptr;
			case RendererAPI::OpenGL:   return new OpenGLVertexArray();
		}

		E_CORE_ASSERT(false, "Unknown RendererAPI.");
		return nullptr;

	}
}