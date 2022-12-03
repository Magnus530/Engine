#pragma once
#include <glm/glm.hpp>
namespace Engine {

	class RayCast
	{
	public:
		static void OnWindowResize(unsigned int width, unsigned int height);

	public:	// Raycast functions
		static void FromScreenPosition(glm::vec3& ray, glm::vec2 screenPosition, glm::mat4 projection, glm::mat4 view);

		static bool IntersectWithWAPlaneXZ(glm::vec3& intersect_OUT, const glm::vec3& ray, const glm::vec3& pos);
		static bool IntersectWithWAPlaneXY(glm::vec3& intersect_OUT, const glm::vec3& ray, const glm::vec3& pos);
		static bool IntersectWithWAPlaneZY(glm::vec3& intersect_OUT, const glm::vec3& ray, const glm::vec3& pos);

		static bool IntersectWithAlignedPlane(glm::vec3& intersect_OUT, const glm::vec3& planeNormal, const glm::vec3& planePosition, const glm::vec3& ray, const glm::vec3& rayPosition);

		static bool IntersectSphere(glm::vec3& intersect_OUT, const glm::vec3& ray, const glm::vec3& rayPosition, const glm::vec3& spherePosition, const float& sphereRadius);

	private: // IMPL funcitons
		static void IntersectWithWAPlaneXZ_IMPL(glm::vec3& intersect_OUT, const glm::vec3& ray, const glm::vec3& pos);
		static void IntersectWithWAPlaneXY_IMPL(glm::vec3& intersect_OUT, const glm::vec3& ray, const glm::vec3& pos);
		static void IntersectWithWAPlaneZY_IMPL(glm::vec3& intersect_OUT, const glm::vec3& ray, const glm::vec3& pos);

	private:
		struct WindowSize
		{
			unsigned int X = 1280;
			unsigned int Y = 720;
		};
		static WindowSize* m_WindowSize;
	};
}