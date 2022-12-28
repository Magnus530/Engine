#pragma once

#include "Platform/OpenGL/CubemaptestGL.h"

namespace Engine
{
	class CubemapMid
	{
	public:
		CubemapMid()
		{}

		static void start()
		{
			m_cubemap->start();
		}
		static void CubemapRender(Engine::Renderer::SceneData* scene)
		{
			m_cubemap->skyrender(scene);
		}
	private:
		static CubemaptestGL* m_cubemap;
	};
}