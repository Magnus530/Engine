#include "epch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Engine
{
	//Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:     E_CORE_ASSERT(false, "RendererAPI::None has yet to be supported."); return nullptr;
			case RendererAPI::API::OpenGL:   return new OpenGLVertexBuffer(vertices, size);
		}
		E_CORE_ASSERT(false, "Unknown RendererAPI.");
		return nullptr;
	}

	//Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size)
	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:     E_CORE_ASSERT(false, "RendererAPI::None has yet to be supported."); return nullptr;
			case RendererAPI::API::OpenGL:   return new OpenGLIndexBuffer(indices, size);
		}

		E_CORE_ASSERT(false, "Unknown RendererAPI.");
		return nullptr;
	}
}