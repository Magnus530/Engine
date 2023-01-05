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
		static void MakeKnotVector(BSpline& bSpline);
		static glm::vec3 GetLocationAlongSpline(BSpline& bSpline, float t);

		static float Bid(std::vector<float>& knotVector, float t, int it, int d);
		static float Wid(std::vector<float>& knotVector, float t, int it, int d);
	};
}