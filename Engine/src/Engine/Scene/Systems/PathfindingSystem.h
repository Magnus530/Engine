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
		NodeGrid(glm::vec3 location, glm::vec3 extent, int estimatedAmountofNodes = 1000)
			: m_Location{ location }, m_Extent{ extent }
		{
			m_NodeLocations = new std::vector<glm::vec3>();
			m_NodeDN = new PathNodes_DN();
			m_NodePathConnection = new std::vector<int>();
			m_NodeObstructionStatus = new std::vector<bool>();

			m_NodeLocations->reserve(estimatedAmountofNodes);
			m_NodeDN->m_DistanceValues.reserve(estimatedAmountofNodes);
			m_NodeDN->m_NeighborIndices.reserve(estimatedAmountofNodes);
			m_NodePathConnection->reserve(estimatedAmountofNodes);
			m_NodeObstructionStatus->reserve(estimatedAmountofNodes);
		}
		~NodeGrid()
		{
			delete m_NodeLocations;
			delete m_NodeDN;
			delete m_NodePathConnection;
			delete m_NodeObstructionStatus;
		}
		// Grid 
		glm::vec3 m_Location;
		glm::vec3 m_Extent;
		std::vector<int>* mm;
		
		//std::vector<std::shared_ptr<PNode>> m_Nodes;
		// Node 
			//	Position 
			//	DistanceValues /F - G - H/ & Neighbors
			//  Node Connection
			//	Obstruction
		std::vector<glm::vec3>* m_NodeLocations;
		PathNodes_DN* m_NodeDN;
		std::vector<int>* m_NodePathConnection;
		std::vector<bool>* m_NodeObstructionStatus;
		
		// Obstruction shapes
		PathObstructionSphereCollection m_ObstructionSpheres;


		int AddNode(glm::vec3 location)
		{
			m_NodeLocations->push_back(location);
			m_NodeDN->m_DistanceValues.push_back({ 0,0,0 });
			m_NodeDN->m_NeighborIndices.push_back(std::vector<int>{});
			m_NodePathConnection->push_back(0);
			m_NodeObstructionStatus->push_back(false);

			return m_NodeLocations->size();
		}
		void ConnectNode(int node, int connection)
		{
			m_NodeDN->m_NeighborIndices[node].push_back(connection);
		}
		void NodeInitValues(int node, int targetNode)
		{
			auto& dNode = m_NodeDN->m_DistanceValues;
			dNode[node].H = GetNodeDistanceToNode(node, targetNode);
			dNode[node].F = dNode[node].G + dNode[node].H;
		}
		int GetDistanceToNode(glm::vec3 location, int targetNode)
		{
			return (int)(glm::length(location - m_NodeLocations->at(targetNode)) * PATH_FLOATTOINT);
		}
		int GetNodeDistanceToNode(int node, int targetNode)
		{
			return (int)(glm::length(m_NodeLocations->at(node) - m_NodeLocations->at(targetNode)) * PATH_FLOATTOINT);
		}
	};
#ifdef E_PATHNODE_DEBUG
	inline NodesAdditional* m_NodesAdditional = new NodesAdditional();
#endif


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
		//static std::shared_ptr<PNode> GetNodeClosestToPosition(uint32_t gridIndex, glm::vec3 position);
		static int GetNodeClosestToPosition(uint32_t gridIndex, glm::vec3 position);

	private:
		//static std::vector<std::shared_ptr<PNode>> FindPathAStar(std::shared_ptr<PNode> start, std::shared_ptr<PNode> end, std::shared_ptr<PNode>& intermediate, bool* blocked = nullptr);
		static std::vector<int> FindPathAStar(int gridIndex, int startNode, int endNode, int& intermediateNode, bool* blocked = nullptr);
	};


	struct ObstructionUpdates
	{
		std::vector<int> m_ObstructionSpheres;
		std::vector<int> m_CurrentSphereNodes;
		std::vector<int> m_PotensiallyFalseObstructionNodes;

		void AddNode_Potensial(int node)
		{
			if (std::find(m_PotensiallyFalseObstructionNodes.begin(), m_PotensiallyFalseObstructionNodes.end(), node) == m_PotensiallyFalseObstructionNodes.end())
				m_PotensiallyFalseObstructionNodes.push_back(node);
		}
		void AddNode_Current(int node)
		{
			if (std::find(m_CurrentSphereNodes.begin(), m_CurrentSphereNodes.end(), node) == m_CurrentSphereNodes.end())
				m_CurrentSphereNodes.push_back(node);
		}
	};
	// Creation of Nodegrids
	class NodeGridSystem
	{
	public:
		//static void CreateGrid();
		static void CreateGridAtLocation(glm::vec3 location, glm::vec3 extent, int resolution);
		static std::shared_ptr<NodeGrid> GetGridAtIndex(uint32_t index);

		static glm::vec3 GetNodeLocation(int gridIndex, int NodeIndex);

		//static std::shared_ptr<PNode> GetNodeAtIndexWithinGrid(uint32_t gridIndex, uint32_t nodeIndex);

		static uint32_t CreateObstructionSphere(uint32_t gridIndex, float radius, glm::vec3 location);
		static void DeleteObstructionSphere(uint32_t gridIndex, uint32_t sphereIndex);
		static void UpdateObstructionSphere(uint32_t gridIndex, uint32_t sphereIndex, float radius, glm::vec3 location);
		static void UpdateFalseObstructionNodes(uint32_t gridIndex);

	private:
		// Generate names for the nodes within a Node Grid
		//static void GenerateNodeNamesForGrid(std::shared_ptr<NodeGrid> grid);
		
		// Obstruction Volumes
		
	};

}