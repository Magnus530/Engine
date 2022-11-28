#pragma once
#include "glm/glm.hpp"

namespace Engine {
	inline float floatConvert{ 1000.f };

	//enum ENodeType : unsigned short	// redundant
	//{
	//	NT_None,
	//	NT_Start,
	//	NT_Path,
	//	NT_Target,
	//	NT_Checked,
	//	NT_Processed,
	//	NT_Block
	//};

	struct PathNodeNeighbors
	{
		std::vector<std::shared_ptr<class PNode>> neighbors;
	};

	struct PathNodeDistanceValues
	{
		int F;	// Total DistanceValue
		int G;	// DistanceValue from StartNode to this
		int H;	// DistanceValue from EndNode to this
	};

	struct PathNodeData
	{
		//const char* m_Name;
		std::string m_Name;
		glm::vec3 m_Position;
		bool bIsObstruction{};
	};

	class PNode
	{
	public:
		PNode(glm::vec3 position)
		{
			//m_Data = new PathNodeData();
			//m_Neighbors = new PathNodeNeighbors();
			m_Data = std::make_shared<PathNodeData>();
			m_Neighbors = std::make_shared<PathNodeNeighbors>();
			m_DistanceValues = std::make_shared<PathNodeDistanceValues>();

			m_Data->m_Position = position;
		}
		PNode(std::string name, glm::vec3 position) 
			: PNode(position)
		{
			m_Data->m_Name = name;
		}
		~PNode()
		{
		}

		std::shared_ptr<PathNodeData> m_Data;
		//std::string m_name;
		//glm::vec3 m_Position;
		//ENodeType NodeType = ENodeType::NT_None;

		std::shared_ptr<PathNodeNeighbors> m_Neighbors;

		//PathNodeDirectConnection* m_DirectConnection;
		/* Direct connection to next node when moving through the path from the end node */
		//std::shared_ptr<PNode> m_Connection;
		std::shared_ptr<PNode> m_Connection;
		//std::vector<std::shared_ptr<PNode>> mConnectedNodes;

		std::shared_ptr<PathNodeDistanceValues> m_DistanceValues;
		//int F;	// Total DistanceValue
		//int G;	// DistanceValue from StartNode to this
		//int H;	// DistanceValue from EndNode to this

		void AddConnectedNode(std::shared_ptr<PNode> node) { 
			m_Neighbors->neighbors.push_back(node);
			//mConnectedNodes.push_back(node); 
		}

		int GetDistanceToNode(PNode* target)
		{
			return (int)(glm::length(m_Data->m_Position - target->m_Data->m_Position) * floatConvert);
		}
		void InitValues(PNode* target)
		{
			m_DistanceValues->H = GetDistanceToNode(target);
			m_DistanceValues->F = m_DistanceValues->G + m_DistanceValues->H;
		}

		void SetG(int g) { m_DistanceValues->G = g;  m_DistanceValues->F = m_DistanceValues->G + m_DistanceValues->H; }
		void SetH(PNode* target) { m_DistanceValues->H = GetDistanceToNode(target); }

		bool IsBlock() const { 
			return m_Data->bIsObstruction;
			//return NodeType == ENodeType::NT_Block; 
		}
		void SetBlock(bool b)
		{
			m_Data->bIsObstruction = true;
		}
	};

//	namespace Pathfinder {
//		// B-Spline
//		inline int m_SplineDegree = 2;
//		inline std::vector<float> m_Knotvector;
//		glm::vec3 GetPositionAlongSpline(std::vector<glm::vec3>& splinepoints, float t);	// B-Spline through path
//		void MakeKnotVector(std::vector<glm::vec3> nodelocations);
//		float Bid(float t, int it, int d);
//		float Wid(float t, int it, int d);
//
//		// Pathfinding 
//		std::vector<PNode*> FindPath(std::shared_ptr<PNode> start, std::shared_ptr<PNode> end);
//		glm::vec3 GetPositionofNode(uint32_t index);
//		std::shared_ptr<PNode> GetNodeAtIndex(uint32_t index);
//		std::shared_ptr<PNode> GetNodeClosestToPosition(glm::vec3 position);
//	
//		// Node Spawn 
//		inline int GridSize{ 10 };
//		inline float GridSpacing{ 1.f };
//		inline glm::vec3 Center(0, 0, 0);
//		inline std::vector<std::shared_ptr<PNode>> m_Nodes;
//		inline std::vector<glm::vec3> m_NodeLocations;
//		inline std::string GenerateNodeName()
//		{
//			std::string name = "PNode ";
//			name += std::to_string(m_Nodes.size()+1);
//			return name;
//		}
//		inline void SpawnGrid()
//		{
//			std::shared_ptr<PNode> a;
//			std::shared_ptr<PNode> b;
//
//			for (size_t x{}; x < 10; x++)
//			{
//				for (size_t z{}; z < 10; z++)
//				{
//					glm::vec3 location = glm::vec3(Center.x + (GridSpacing * x) - (float)GridSize / 2, 0, Center.z + (GridSpacing * z) - (float)GridSize / 2);
//					m_Nodes.emplace_back(std::make_shared<PNode>(GenerateNodeName(), location));
//					m_NodeLocations.push_back(location);
//				}
//			}
//			for (size_t i{1}; i < m_Nodes.size(); i++)
//			{
//				if (m_Nodes.size() < i + 1) { break; }
//
//				a = m_Nodes[i - 1];
//				b = m_Nodes[i];
//				if (i%10!=0 || i==0) {
//					a->AddConnectedNode(b);
//					b->AddConnectedNode(a);
//				}
//
//				if (m_Nodes.size() < i + 10) { continue; }
//				b = m_Nodes[(i-1) + 10];
//				b->AddConnectedNode(a);
//				a->AddConnectedNode(b);
//			}
//		}
//	}
}