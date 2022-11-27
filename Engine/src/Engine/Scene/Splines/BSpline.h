#pragma once
#include <Engine.h>

namespace Engine {

	struct Spline
	{
		std::vector<glm::vec3> m_Points;
	};

	struct BSpline
	{
		int m_Degree { 2 };
		std::vector<float> m_KnotVector;
		std::vector<glm::vec3> m_Controlpoints;
	};

	class BSplineCreator
	{
	public:
		std::vector<float> MakeKnotVector(std::vector<glm::vec3> controlpoints);

		static float Bid(float t, int it, int d);
		static float Wid(float t, int it, int d);

	private:
		static std::vector<float> m_KnotVector;
	};
}