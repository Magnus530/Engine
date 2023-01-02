#include "epch.h"
#include "RayCast.h"
#include <glm/gtx/projection.hpp>

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
	bool RayCast::IntersectWithWAPlaneXZ(glm::vec3& intersect_OUT, const glm::vec3& ray, const glm::vec3& pos)
	{
		if ((ray.y >= 0.f && pos.y >= 0.f) || (ray.y <= 0.f && pos.y <= 0.f))
			return false;
		
		IntersectWithWAPlaneXZ_IMPL(intersect_OUT, ray, pos);
		return true;
	}
	bool RayCast::IntersectWithWAPlaneXY(glm::vec3& intersect_OUT, const glm::vec3& ray, const glm::vec3& pos)
	{
		if ((ray.z >= 0.f && pos.z >= 0.f) || (ray.z <= 0.f && pos.z <= 0.f))
			return false;

		IntersectWithWAPlaneXY_IMPL(intersect_OUT, ray, pos);
		return true;
	}

	bool RayCast::IntersectWithWAPlaneZY(glm::vec3& intersect_OUT, const glm::vec3& ray, const glm::vec3& pos)
	{
		if ((ray.x >= 0.f && pos.x >= 0.f) || (ray.x <= 0.f && pos.x <= 0.f)) 
			return false;
		
		IntersectWithWAPlaneZY_IMPL(intersect_OUT, ray, pos);
		return true;
	}

	bool RayCast::IntersectWithAlignedPlane(glm::vec3& intersect_OUT, const glm::vec3& planeNormal, const glm::vec3& planePosition, const glm::vec3& ray, const glm::vec3& rayPosition)
	{
		float u = glm::dot((planePosition - rayPosition), planeNormal);
		float l = glm::dot(ray, planeNormal);
		float t = u / l;
		if (t <= 0.f)
			return false;

		glm::vec3 loc = rayPosition + (ray * t);
		intersect_OUT = loc;
		return true;
	}

	bool RayCast::IntersectWithPlane(glm::vec3& intersect_OUT, const glm::vec3 planeNormal, const glm::vec3 vertexPosition, const glm::vec3 planeVector, const glm::vec3 ray, const glm::vec3 rayPosition)
	{
		if (!IntersectWithAlignedPlane(intersect_OUT, planeNormal, vertexPosition, ray, rayPosition))
			return false;

		auto proj = glm::proj(intersect_OUT - vertexPosition, planeVector);
		if (glm::length(proj) > glm::length(planeVector))
			return false;
		if (glm::dot(glm::normalize(proj), glm::normalize(planeVector)) < 0.f)
			return false;

		intersect_OUT = vertexPosition + proj;
		return true;
	}

	bool RayCast::IntersectSphere(glm::vec3& intersect_OUT, const glm::vec3& ray, const glm::vec3& rayPosition, const glm::vec3& spherePosition, const float& sphereRadius)
	{
		float tr = glm::dot(spherePosition - rayPosition, ray);
		float y = glm::length(spherePosition - (rayPosition + ray * tr));
		if (y < sphereRadius) 
		{
			float x = sqrt(pow(sphereRadius, 2) - pow(y, 2));
			intersect_OUT = rayPosition + ray * (tr-x);
			return true;
		}
		return false;
	}

	/*------------------------------------- IMPL FUNCTIONS --------------------------------------*/
	void RayCast::IntersectWithWAPlaneXZ_IMPL(glm::vec3& intersect_OUT, const glm::vec3& ray, const glm::vec3& pos)
	{
		intersect_OUT.y = 0.f;
		intersect_OUT.x = ((-pos.y * ray.x) / ray.y) + pos.x;
		intersect_OUT.z = ((-pos.y * ray.z) / ray.y) + pos.z;
	}
	void RayCast::IntersectWithWAPlaneXY_IMPL(glm::vec3& intersect_OUT, const glm::vec3& ray, const glm::vec3& pos)
	{
		intersect_OUT.z = 0.f;
		intersect_OUT.x = ((-pos.z * ray.x) / ray.z) + pos.x;
		intersect_OUT.y = ((-pos.z * ray.y) / ray.z) + pos.y;
	}
	void RayCast::IntersectWithWAPlaneZY_IMPL(glm::vec3& intersect_OUT, const glm::vec3& ray, const glm::vec3& pos)
	{
		intersect_OUT.x = 0.f;
		intersect_OUT.z = ((-pos.x * ray.z) / ray.x) + pos.z;
		intersect_OUT.y = ((-pos.x * ray.y) / ray.x) + pos.y;
	}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ------------------------------------------------------ COLLISION SYSTEM ------------------------------------------------------------------- //

	// ----- SAT ----- //
	bool CollisionSystem::IsColliding(glm::vec3 point, ConvexPolygon& a)
	{
		return FindMinSeparation(point, a) <= 0;
	}

	bool CollisionSystem::IsColliding(ConvexPolygon& a, ConvexPolygon& b)
	{
		return FindMinSeparation(a, b) <= 0.f && FindMinSeparation(b, a) <= 0.f;
	}

	float CollisionSystem::FindMinSeparation(ConvexPolygon& a, ConvexPolygon& b)
	{
		float separation = std::numeric_limits<float>::lowest();

		// Loop through all normals in polygon "a"
		for (int i = 0; i < a.m_Locations.size(); i++)
		{
			glm::vec3 location = a.m_Locations[i];
			glm::vec3 normal = a.m_Normals[i];
			float minSep = std::numeric_limits<float>::max();

			for (glm::vec3 l : b.m_Locations)
				minSep = std::min(minSep, glm::dot(l - location, normal));

			if (minSep > separation)
				separation = minSep;
		}

		return separation;
	}
	float CollisionSystem::FindMinSeparation(glm::vec3 point, ConvexPolygon& a)
	{
		float separation = std::numeric_limits<float>::lowest();

		for (int i = 0; i < a.m_Locations.size(); i++)
		{
			glm::vec3 location = a.m_Locations[i];
			glm::vec3 normal = a.m_Normals[i];
			float minSep = std::numeric_limits<float>::max();

			minSep = std::min(minSep, glm::dot(point - location, normal));

			if (minSep > separation)
				separation = minSep;
		}
		return separation;
	}
}