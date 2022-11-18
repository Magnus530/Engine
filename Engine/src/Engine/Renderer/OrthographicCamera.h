#pragma once

#include "Camera.h"

#include <glm/glm.hpp>

namespace Engine
{
	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);
		
		void SetProjection(float left, float right, float bottom, float top);
	};
}
