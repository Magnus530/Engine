#include "epch.h"
#include "BSpline.h"

namespace Engine {

	void BSplineCreator::MakeKnotVector(BSpline& bspline)
	{
		bspline.m_KnotVector.clear();
		// Is data now doubleb up?
		std::vector<float> knots;
		std::vector<glm::vec3> nodelocations = bspline.m_Controlpoints;
		int splineDegree = bspline.m_Degree;

		if (nodelocations.size() == 3) {
			bspline.m_KnotVector = { 0, 0, 0, 1, 1, 1 };
			return;
		}

		/* Knot vector */
		int knotpoints = nodelocations.size() + splineDegree + 1;
		float part = (float)knotpoints - ((splineDegree + 1) * 2);
		float step = (1 / (part + 1));

		for (int i{ 1 }; i <= knotpoints; i++)
		{
			if (i <= (splineDegree + 1)) {
				knots.push_back(0);
			}
			else if (i > knotpoints - (splineDegree + 1)) {
				knots.push_back(1);
			}
			else {
				int j = i - (splineDegree + 1);
				float knot = step * j;
				knots.push_back(knot);
			}
		}
		bspline.m_KnotVector = knots;
	}

	glm::vec3 BSplineCreator::GetLocationAlongSpline(BSpline& bSpline, float t)
	{
		if (bSpline.m_Controlpoints.size() == 0) {
			return glm::vec3();
		}

		glm::vec3 pos{};
		for (size_t j{}; j < bSpline.m_Controlpoints.size(); j++) {
			pos += bSpline.m_Controlpoints[j] * Bid(bSpline.m_KnotVector, t, j, bSpline.m_Degree);
		}
		return pos;
	}

	float BSplineCreator::Bid(std::vector<float>& knotVector, float t, int it, int d)
	{
		if (d == 0) {
			if (knotVector[it] <= t && t < knotVector[it + 1]) {
				return 1.f;
			}
			return 0.0f;
		}
		return (Wid(knotVector, t, it, d) * Bid(knotVector, t, it, d - 1)) + ((1 - Wid(knotVector, t, it + 1, d)) * Bid(knotVector, t, it + 1, d - 1));
	}

	float BSplineCreator::Wid(std::vector<float>& knotVector, float t, int it, int d)
	{
		if (knotVector[it] < knotVector[it + d]) {
			return (t - knotVector[it]) / (knotVector[it + d] - knotVector[it]);
		}
		return 0.0f;
	}
}