#pragma once
#include <Engine.h>
#include "../Components.h"
#include "../../Pathfinding/PNode.h"
#include "../Splines/BSpline.h"
#include "TransformSystem.h"
#include "../../Pathfinding/PathObstructionVolumes.h"

namespace Engine {

	struct NodeGrid
	{
		NodeGrid(glm::vec3 position, glm::vec3 extent, std::vector<std::shared_ptr<PNode>>& nodes)
			: m_Position{ position }, m_Extent{ extent }, m_Nodes{ nodes } {}

		glm::vec3 m_Position;
		glm::vec3 m_Extent;
		std::vector<std::shared_ptr<PNode>> m_Nodes;
		class PathObstructionSphereCollection m_ObstructionSpheres;
	};


	// Given a Nodegrid, finds a path from A to B
	class PathfindingSystem
	{
	public:
		static void FindPath(PathfindingComponent& comp, const glm::vec3 currentPosition);
		// Deciding type of PathMovement - Regular, Patrol ... etc
		static void PathMovement(PathfindingComponent& pathfinder, TransformComponent& transform, float deltatime);

		// General Movement
		static void MoveAlongPath(PathfindingComponent& pathfinder, TransformComponent& transform, float deltatime);
		static void ResumeMovementAlongPath(PathfindingComponent& pathfinder, const glm::vec3 currentPosition);
		static void CancelMovementAlongPath(PathfindingComponent&);

		// Patrol
		static void MoveAlongPatrol(PathfindingComponent& pathfinder, TransformComponent& transform, float deltatime);
		static void StartPatrol(PathfindingComponent& pathfinder, const glm::vec3 currentPosition, PatrolType patroltype = PatrolType::Loop);
		static void PatrolUpdate(PathfindingComponent& pathfinder, const glm::vec3 currentPosition);

		static void PausePatrol(PathfindingComponent& pathfinder, const glm::vec3 currentPosition);
		static void ResumePatrol(PathfindingComponent& pathfinder, const glm::vec3 currentPosition, PatrolType patroltype = PatrolType::Loop);

		static void AddPatrolPath(PathfindingComponent& pathfinder, std::vector<glm::vec3> patrolpath);
		static void AddToPatrolPath(PathfindingComponent& pathfinder, glm::vec3 pos);
		static void AddToPatrolPathAt(PathfindingComponent& pathfinder, glm::vec3 pos, unsigned int index);
		static void ClearPatrol(PathfindingComponent& pathfinder);

		// 
		static std::shared_ptr<PNode> GetNodeClosestToPosition(uint32_t gridIndex, glm::vec3 position);

	private:
		static std::vector<std::shared_ptr<PNode>> FindPathAStar(std::shared_ptr<PNode> start, std::shared_ptr<PNode> end, std::shared_ptr<PNode>& intermediate, bool* blocked = nullptr);
	};


	// Creation of Nodegrids
	class NodeGridSystem
	{
	public:
		static void CreateGrid();
		static void CreateGridAtLocation(glm::vec3 location, glm::vec3 extent, int resolution);
		static std::shared_ptr<NodeGrid> GetGridAtIndex(uint32_t index);

		static std::shared_ptr<PNode> GetNodeAtIndexWithinGrid(uint32_t gridIndex, uint32_t nodeIndex);

		static uint32_t CreateObstructionSphere(uint32_t gridIndex, float radius, glm::vec3 position);
		static void DeleteObstructionSphere(uint32_t gridIndex, uint32_t sphereIndex);
		static void UpdateObstructionSphere(uint32_t gridIndex, uint32_t sphereIndex, float radius, glm::vec3 position);
		static void UpdateFalseObstructionNodes(uint32_t gridIndex);

	private:
		// Generate names for the nodes within a Node Grid
		static void GenerateNodeNamesForGrid(std::shared_ptr<NodeGrid> grid);
		
		// Obstruction Volumes
		
	};

}