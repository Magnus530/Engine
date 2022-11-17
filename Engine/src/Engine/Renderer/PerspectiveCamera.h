#pragma once

#include <glm/glm.hpp>

#include "Camera.h"

namespace Engine
{
	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera(float fov, float aspectRatio, float nearPlane, float farPlane);
	};
}