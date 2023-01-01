#pragma once

#include "Engine/Renderer/Renderer.h"
#include "Engine/Scene/Scene.h"

namespace Engine
{
	class TextureList
	{
	public:
		TextureList() = default;

		static void CreateTextures(std::shared_ptr<Engine::Scene> scene);

	};
}