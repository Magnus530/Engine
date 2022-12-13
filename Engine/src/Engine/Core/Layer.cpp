#include "epch.h"
#include "Layer.h"

namespace Engine
{
	Layer::Layer(const std::string& debugName)
		: m_DebugName(debugName)
	{}

	Layer::~Layer()
	{}
	//glm::vec3 Layer::RayCastFromScreenPosition(glm::vec2 screenPosition, glm::mat4 projection, glm::mat4 view, glm::vec2 windowSize)
	//{
	//	// Raytrace from screen into the world https://antongerdelan.net/opengl/raycasting.html
	//	// 1: 3D Normalised Device Coordinates
	//	glm::vec3 ray_nds;
	//	ray_nds.x = (2.f * screenPosition.x) / (windowSize.x) - 1.f;
	//	ray_nds.y = 1.f - (2.f * screenPosition.y) / windowSize.y;
	//	ray_nds.z = 1.f;

	//	// 2: Homogenous Clip Coordinates
	//	glm::vec4 ray_clip(ray_nds.x, ray_nds.y, -1.f, 1.f);

	//	// 3: Eye(Camera) Coordinates
	//	glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
	//	ray_eye.z = -1.f; ray_eye.w = 0.f;

	//	// 4: 4D World Coordinates
	//	glm::vec3 ray_world = glm::inverse(view) * ray_eye;
	//	glm::normalize(ray_world);
	//	return ray_world;
	//}
}