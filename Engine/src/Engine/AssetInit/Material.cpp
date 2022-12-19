#include "epch.h"
#include "Material.h"

namespace Engine
{
	FlatMaterial::FlatMaterial(glm::mat4 transform, std::shared_ptr<Engine::VertexArray> va)
		: m_Transform(transform), m_VA(va)
	{}

	FlatMaterial::~FlatMaterial()
	{}
}