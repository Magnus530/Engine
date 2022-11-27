#include "epch.h"
#include "BSpline.h"

namespace Engine {

	std::vector<float> BSplineCreator::MakeKnotVector(std::vector<glm::vec3> controlpoints)
	{
		return std::vector<float>();
	}

	float BSplineCreator::Bid(float t, int it, int d)
	{
		if (d == 0) {
			if (m_KnotVector[it] <= t && t < m_KnotVector[it + 1]) {
				return 1.f;
			}
			return 0.0f;
		}
		return (Wid(t, it, d) * Bid(t, it, d - 1)) + ((1 - Wid(t, it + 1, d)) * Bid(t, it + 1, d - 1));
	}

	float BSplineCreator::Wid(float t, int it, int d)
	{
		if (m_KnotVector[it] < m_KnotVector[it + d]) {
			return (t - m_KnotVector[it]) / (m_KnotVector[it + d] - m_KnotVector[it]);
		}
		return 0.0f;
	}
}