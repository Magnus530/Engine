#include "epch.h"
#include "OpenGLCubemap.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace Engine
{
	OpenGLCubemap::OpenGLCubemap(const std::string cubeArr[])
		: m_Path(cubeArr)
	{
		unsigned int cubemapTexture;
		glGenTextures(1, &cubemapTexture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		for (unsigned int i = 0; i < 6; i++)
		{
			int width, height, channels;
			unsigned char* data = stbi_load(cubeArr[i].c_str(), &width, &height, &channels, 0);

			if (data)
			{
				stbi_set_flip_vertically_on_load(false);
				glTexImage2D
				(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0,
					GL_RGB,
					width,
					height,
					0,
					GL_RGB,
					GL_UNSIGNED_BYTE,
					data
				);
				stbi_image_free(data);
			}
			else
			{
				E_TRACE("Skybox texture failed to load: {0}", cubeArr[i]);
				stbi_image_free(data);
			}
		}

		m_CubemapTexture = cubemapTexture;
	}

	OpenGLCubemap::~OpenGLCubemap()
	{
		glDeleteTextures(1, &m_RendererID);
	}
	void OpenGLCubemap::Bind(uint32_t slot) const
	{
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);
		glBindTexture(GL_TEXTURE_CUBE_MAP, slot);
	}
}