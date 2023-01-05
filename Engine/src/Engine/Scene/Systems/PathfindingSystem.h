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
		NodeGrid(glm::vec3 location, glm::vec3 extent, int resolution, 
			int estimatedAmountofNodes = 1000, bool bDebugRenderEnabled = false)
			: m_Location{ location }, m_Extent{ extent }, 
			Resolution{ resolution }, bRenderNodegrid{ bDebugRenderEnabled }
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
		glm::ivec3 m_Extent;
		int Resolution{ 1 };

		//DEBUG
		bool bRenderNodegrid{};
		
		// Node s
		std::vector<glm::vec3>* m_NodeLocations;
		PathNodes_DN* m_NodeDN;
		std::vector<int>* m_NodePathConnection;
		std::vector<bool>* m_NodeObstructionStatus;
		
		// Obstruction spheres
		PathObstructionSphereCollection m_ObstructionSpheres;


		int AddNode(glm::vec3 location)
		{
			m_NodeLocations->push_back(location);
			m_NodeDN->m_DistanceValues.push_back({ 0,0,0 });
			m_NodeDN->m_NeighborIndices.push_back(std::vector<int>{});
			m_NodePathConnection->push_back(0);
			m_NodeObstructionStatus->push_back(false);

			return (int)m_NodeLocations->size();
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


	// Given a Nodegrid, finds a path from A to B
	class PathfindingSystem
	{
	public:
		static void FindPath(Scene* scene, PathfindingComponent& comp, const glm::vec3 currentPosition);
		// Deciding type of PathMovement - Regular, Patrol ... etc

		// General Movement
		static void MoveAlongPath(Scene* scene, PathfindingComponent& pathfinder, TransformComponent& transform, float deltatime);
		static void ResumeMovementAlongPath(Scene* scene, PathfindingComponent& pathfinder, const glm::vec3 currentPosition);
		static void CancelMovementAlongPath(Scene* scene, PathfindingComponent&);

		// Patrol
		static void MoveAlongPatrol(Scene* scene, PathfindingComponent& pathfinder, TransformComponent& transform, float deltatime);
		static void StartPatrol(Scene* scene, PathfindingComponent& pathfinder, const glm::vec3 currentPosition, PatrolType patroltype = PatrolType::Loop);
		static void PatrolUpdate(Scene* scene, PathfindingComponent& pathfinder, const glm::vec3 currentPosition);

		static void PausePatrol(Scene* scene, PathfindingComponent& pathfinder, const glm::vec3 currentPosition);
		static void ResumePatrol(Scene* scene, PathfindingComponent& pathfinder, const glm::vec3 currentPosition, PatrolType patroltype = PatrolType::Loop);

		static void AddPatrolPath(PathfindingComponent& pathfinder, std::vector<glm::vec3> patrolpath);
		static void AddToPatrolPath(PathfindingComponent& pathfinder, glm::vec3 pos);
		static void AddToPatrolPathAt(PathfindingComponent& pathfinder, glm::vec3 pos, unsigned int index);
		static void ClearPatrol(PathfindingComponent& pathfinder);

		static int GetNodeClosestToPosition(Scene* scene, glm::vec3 position);

	private:
		static std::vector<int> FindPathAStar(Scene* scene, int startNode, int endNode, int& intermediateNode, bool* blocked = nullptr);
	};


	struct ObstructionUpdates
	{
		std::vector<int> m_ObstructionSpheres;	// Spheres that are checked for this ObstructionUpdate
		std::vector<int> m_CurrentSphereNodes;	// Nodes currently within obstruction spheres
		std::vector<int> m_PotensiallyFalse;	// Obstruction nodes that are potentially false

		void AddNode_PotensiallyFalse(int node)
		{
			if (std::find(m_PotensiallyFalse.begin(), m_PotensiallyFalse.end(), node) == m_PotensiallyFalse.end())
				m_PotensiallyFalse.push_back(node);
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
		static void CreateGridAtLocation(Scene* scene, glm::vec3 location, glm::ivec3 extent, int resolution, bool bDebugRenderEnabled = false);

		static glm::vec3 GetNodeLocation(Scene* scene, int NodeIndex);

		static uint32_t CreateObstructionSphere(Scene* scene, float radius, glm::vec3 location);
		static void DeleteObstructionSphere(Scene* scene, uint32_t sphereIndex);
		static void UpdateObstructionSphere(Scene* scene, uint32_t sphereIndex, float radius, glm::vec3 location);
		static void UpdateFalseObstructionNodes(Scene* scene);
	};

}