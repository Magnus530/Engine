#pragma once

#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/Renderer.h"

namespace Engine
{
	class OpenGLCubemap : public TextureCubemap
	{
	public:
		OpenGLCubemap(const std::string cubeArr[]);

		virtual ~OpenGLCubemap();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }

		virtual void Bind(uint32_t slot = 0) const override;

		unsigned int m_CubemapTexture = 0;
	private:
		const std::string* m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
	};
}