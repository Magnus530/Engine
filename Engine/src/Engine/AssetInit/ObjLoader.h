#pragma once

#include <glm/glm.hpp>
#include "Engine/Renderer/vertex.h"

namespace Engine
{
	class ObjLoader
	{
	public:
		static void ReadFile(std::string filename, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
	};
}