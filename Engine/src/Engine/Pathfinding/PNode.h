#pragma once
#include "glm/glm.hpp"

#define PATH_FLOATTOINT 1000

namespace Engine {

	struct PathNodes_DistanceValues
	{
		int F{};
		int G{};
		int H{};
	};
	struct PathNodes_DN	// DistanceValues & Neighbors
	{
		std::vector<PathNodes_DistanceValues> m_DistanceValues;	// F - G - H
		std::vector<std::vector<int>> m_NeighborIndices;
	};
}