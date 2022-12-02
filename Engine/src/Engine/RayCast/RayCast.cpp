#include "epch.h"
#include "RayCast.h"

namespace Engine {

	RayCast::WindowSize* RayCast::m_WindowSize = new RayCast::WindowSize();

	void RayCast::OnWindowResize(unsigned int width, unsigned int height)
	{
		m_WindowSize->X = width;
		m_WindowSize->Y = height;
	}

	void RayCast::FromScreenPosition(glm::vec3& ray, glm::vec2 screenPosition, glm::mat4 projection, glm::mat4 view)
	{
		glm::vec2 windowSize(m_WindowSize->X, m_WindowSize->Y);
		// Raytrace from screen into the world https://antongerdelan.net/opengl/raycasting.html
		// 1: 3D Normalised Device Coordinates
		glm::vec3 ray_nds;
		ray_nds.x = (2.f * screenPosition.x) / (windowSize.x) - 1.f;
		ray_nds.y = 1.f - (2.f * screenPosition.y) / windowSize.y;
		ray_nds.z = 1.f;

		// 2: Homogenous Clip Coordinates
		glm::vec4 ray_clip(ray_nds.x, ray_nds.y, -1.f, 1.f);

		// 3: Eye(Camera) Coordinates
		glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
		ray_eye.z = -1.f; ray_eye.w = 0.f;

		// 4: 4D World Coordinates
		glm::vec3 ray_world = glm::inverse(view) * ray_eye;
		glm::normalize(ray_world);
		ray = ray_world;
	}

	/*------------------------------------------- INTERSECTIONS ----------------------------------------------*/
	bool RayCast::IntersectionWithPlaneXZ(glm::vec3& intersection_OUT, const glm::vec3& ray, const glm::vec3& pos)
	{
		if ((ray.y >= 0.f && pos.y >= 0.f) || (ray.y <= 0.f && pos.y <= 0.f))
			return false;
		
		IntersectionWithPlaneXZ_IMPL(intersection_OUT, ray, pos);
		return true;
	}
	bool RayCast::IntersectionWithPlaneXY(glm::vec3& intersection_OUT, const glm::vec3& ray, const glm::vec3& pos)
	{
		if ((ray.z >= 0.f && pos.z >= 0.f) || (ray.z <= 0.f && pos.z <= 0.f))
			return false;

		IntersectionWithPlaneXY_IMPL(intersection_OUT, ray, pos);
		return true;
	}

	bool RayCast::IntersectionWithPlaneZY(glm::vec3& intersection_OUT, const glm::vec3& ray, const glm::vec3& pos)
	{
		if ((ray.x >= 0.f && pos.x >= 0.f) || (ray.x <= 0.f && pos.x <= 0.f)) 
			return false;
		
		IntersectionWithPlaneZY_IMPL(intersection_OUT, ray, pos);
		return true;
	}

	/*------------------------------------- IMPL FUNCTIONS --------------------------------------*/
	void RayCast::IntersectionWithPlaneXZ_IMPL(glm::vec3& intersection_OUT, const glm::vec3& ray, const glm::vec3& pos)
	{
		intersection_OUT.y = 0.f;
		intersection_OUT.x = ((-pos.y * ray.x) / ray.y) + pos.x;
		intersection_OUT.z = ((-pos.y * ray.z) / ray.y) + pos.z;
	}
	void RayCast::IntersectionWithPlaneXY_IMPL(glm::vec3& intersection_OUT, const glm::vec3& ray, const glm::vec3& pos)
	{
		intersection_OUT.z = 0.f;
		intersection_OUT.x = ((-pos.z * ray.x) / ray.z) + pos.x;
		intersection_OUT.y = ((-pos.z * ray.y) / ray.z) + pos.y;
	}
	void RayCast::IntersectionWithPlaneZY_IMPL(glm::vec3& intersection_OUT, const glm::vec3& ray, const glm::vec3& pos)
	{
		intersection_OUT.x = 0.f;
		intersection_OUT.z = ((-pos.x * ray.z) / ray.x) + pos.z;
		intersection_OUT.y = ((-pos.x * ray.y) / ray.x) + pos.y;
	}
}