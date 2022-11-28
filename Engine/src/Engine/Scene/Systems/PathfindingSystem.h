#pragma once
#include <Engine.h>
#include "../Components.h"
#include "../../Pathfinding/PNode.h"
#include "../Splines/BSpline.h"
#include "TransformSystem.h"

namespace Engine {

	struct NodeGrid
	{
		NodeGrid(glm::vec3 position, glm::vec3 extent, std::vector<std::shared_ptr<PNode>>& nodes)
			: m_Position{ position }, m_Extent{ extent }, m_Nodes{ nodes } {}

		glm::vec3 m_Position;
		glm::vec3 m_Extent;
		std::vector<std::shared_ptr<PNode>> m_Nodes;
	};

	// Given a Nodegrid, finds a path from A to B
	class PathfindingSystem
	{
	public://Functions
		static void FindPath(PathfindingComponent& comp, const glm::vec3 currentPosition);
		static void MoveAlongPath(PathfindingComponent& pathfinder, TransformComponent& transform, float deltatime);

		static std::shared_ptr<PNode> GetNodeClosestToPosition(uint32_t gridIndex, glm::vec3 position);

	private://Data
		static std::vector<std::shared_ptr<PNode>> FindPath(std::shared_ptr<PNode> start, std::shared_ptr<PNode> end, std::shared_ptr<PNode>& intermediate, bool* blocked = nullptr);

		//static glm::vec3 GetPositionAlongSpline(std::vector<glm::vec3>& splinepoints, float t);
	};

	// Creation of Nodegrids
	class NodeGridSystem
	{
	public:
		// TODO: should be 'grid extent and grid resolution', instead of 'gridSpacid and gridSize'
		static void CreateGrid();
		static void CreateGridAtLocation(glm::vec3 location, float gridSpacing, int gridSize);
		static std::shared_ptr<NodeGrid> GetGridAtIndex(uint32_t index);
		//static std::shared_ptr<NodeGrid> GetGridAtLocation(const glm::vec3& location);

		static std::shared_ptr<PNode> GetNodeAtIndexWithinGrid(uint32_t gridIndex, uint32_t nodeIndex);

	private:
		// Generate names for the nodes within a Node Grid
		static void GenerateNodeNamesForGrid(std::shared_ptr<NodeGrid> grid);
	};

}