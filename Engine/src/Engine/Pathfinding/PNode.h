#pragma once
#include "glm/glm.hpp"

#define E_PATHNODE_DEBUG (E_DEBUG || E_DEVELOPMENT)
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

#ifdef E_PATHNODE_DEBUG
	/* Additional ways of identifying and visualising nodes -- DEBUG ONLY */
	struct NodesAdditional
	{
		std::vector<std::vector<std::string>> m_NodeNames;

		std::shared_ptr<class VertexArray> m_NodeVA;
		float NodePathScale{ 0.8f };
		glm::vec3 NodeColor_Default{ 0.1, 0.7, 0.7 };
		glm::vec3 NodeColor_Path{ 0.8, 0.8, 0.8 };
		glm::vec3 NodeColor_Obstruction{ 0.1, 0.1, 0.1 };
	};
#endif
}