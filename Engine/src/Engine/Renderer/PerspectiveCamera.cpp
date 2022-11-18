#include "epch.h"
#include "PerspectiveCamera.h"

#include "glm/gtc/matrix_transform.hpp"

namespace Engine
{
	PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio, float nearPlane, float farPlane)
	{
		m_ProjectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
		RecalculateViewMatrix();
	}
}