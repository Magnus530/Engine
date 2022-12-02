#pragma once
#include <glm/glm.hpp>
namespace Engine {

	class RayCast
	{
	public:
		static void OnWindowResize(unsigned int width, unsigned int height);

	public:	// Raycast functions
		static void FromScreenPosition(glm::vec3& ray, glm::vec2 screenPosition, glm::mat4 projection, glm::mat4 view);

		static bool IntersectionWithPlaneXZ(glm::vec3& intersection_OUT, const glm::vec3& ray, const glm::vec3& pos);
		static bool IntersectionWithPlaneXY(glm::vec3& intersection_OUT, const glm::vec3& ray, const glm::vec3& pos);
		static bool IntersectionWithPlaneZY(glm::vec3& intersection_OUT, const glm::vec3& ray, const glm::vec3& pos);

	private: // IMPL funcitons
		static void IntersectionWithPlaneXZ_IMPL(glm::vec3& intersection_OUT, const glm::vec3& ray, const glm::vec3& pos);
		static void IntersectionWithPlaneXY_IMPL(glm::vec3& intersection_OUT, const glm::vec3& ray, const glm::vec3& pos);
		static void IntersectionWithPlaneZY_IMPL(glm::vec3& intersection_OUT, const glm::vec3& ray, const glm::vec3& pos);

	private:
		struct WindowSize
		{
			unsigned int X = 1280;
			unsigned int Y = 720;
		};
		static WindowSize* m_WindowSize;
	};
}