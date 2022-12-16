#pragma once

#include "epch.h"
#include "Renderer.h"
#include "Shader.h"

namespace Engine
{
	class RenderStrategy
	{
	public:
		virtual ~RenderStrategy() = default;
		virtual void ShaderSelect() const = 0;
	};
}