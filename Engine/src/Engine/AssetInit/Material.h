#pragma once

#include "Engine/Scene/Entity.h"
#include "Engine/Renderer/VertexArray.h"
#include "glm/glm.hpp"

#include <memory>

namespace Engine
{
	class Material
	{
	public:
		virtual ~Material() = 0;
	};

	class FlatMaterial : public Material
	{
	public:
		FlatMaterial(glm::mat4 transform, std::shared_ptr<Engine::VertexArray> va);
		virtual ~FlatMaterial() override;

		glm::mat4 m_Transform{0};
		std::shared_ptr<Engine::VertexArray> m_VA;
	};
}