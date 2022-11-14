#include "epch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Engine
{
	std::shared_ptr<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:     E_CORE_ASSERT(false, "RendererAPI::None has yet to be supported."); return nullptr;
			case RendererAPI::API::OpenGL:   return std::make_shared<OpenGLTexture2D>(path);
		}

		E_CORE_ASSERT(false, "Unknown RendererAPI.");
		return nullptr;

		return std::shared_ptr<Texture2D>();
	}
}