#include "epch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Engine
{
	/* Creating VERTEX BUFFERS */
	VertexBuffer* VertexBuffer::Create(void* data, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:     E_CORE_ASSERT(false, "RendererAPI::None has yet to be supported."); return nullptr;
		case RendererAPI::API::OpenGL:   return new OpenGLVertexBuffer(data, size);
		}
		E_CORE_ASSERT(false, "Unknown RendererAPI.");
		return nullptr;
	}

	/* Creating INDEX BUFFERS */
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
	IndexBuffer* IndexBuffer::Create(std::vector<uint32_t>& indices)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:     E_CORE_ASSERT(false, "RendererAPI::None has yet to be supported."); return nullptr;
		case RendererAPI::API::OpenGL:   return new OpenGLIndexBuffer(indices);
		}

		E_CORE_ASSERT(false, "Unknown RendererAPI.");
		return nullptr;
	}
}