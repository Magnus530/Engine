#pragma once
#include "glm/glm.hpp"

namespace Engine {
	inline int floatConvert{ 10 };

	enum class ENodeType
	{
		NT_None,
		NT_Start,
		NT_Path,
		NT_Target,
		NT_Checked,
		NT_Processed,
		NT_Block
	};

	class ENGINE_API PNode
	{
	public:
		//PNode();
		PNode(std::string name, glm::vec3 position);
		~PNode();
		std::string m_name;

		ENodeType NodeType = ENodeType::NT_None;

		glm::vec3 m_Position;
		std::vector<std::shared_ptr<PNode>> mConnectedNodes;
		void AddConnectedNode(std::shared_ptr<PNode> node) { mConnectedNodes.push_back(node); }
		/* Direct connection to next node when moving through the path from the end node */
		std::shared_ptr<PNode> m_Connection;

		int F;	// Total DistanceValue
		int G;	// DistanceValue from StartNode to this
		int H;	// DistanceValue from EndNode to this


		int GetDistanceToNode(PNode* target);
		void InitValues(PNode* start, PNode* target);

		void SetG(int g) { G = g; F = G + H; }
		void SetH(PNode* target) { H = GetDistanceToNode(target); }

		bool IsBlock() const { return NodeType == ENodeType::NT_Block; }

	};

	namespace Pathfinder {
		/* B-Spline */
		inline int m_SplineDegree = 2;
		inline std::vector<float> m_Knotvector;
		glm::vec3 GetPositionAlongSpline(std::vector<glm::vec3> splinepoints, float t);
		void MakeKnotVector(std::vector<glm::vec3> nodelocations);
		float Bid(float t, int it, int d);
		float Wid(float t, int it, int d);

		std::vector<PNode*> FindPath(std::shared_ptr<PNode> start, PNode* end);
		
	
		// Spawner noder for testing 
		inline int GridSize{ 10 };
		//inline float GridSpacing{ 0.1f };
		inline glm::vec3 Center(0, 0, 0);
		inline std::vector<std::shared_ptr<PNode>> m_Nodes;
		inline std::vector<glm::vec3> m_NodeLocations;
		inline std::string GenerateNodeName()
		{
			std::string name = "PNode ";
			name += std::to_string(m_Nodes.size());
			return name;
		}
		inline void SpawnGrid()
		{
			std::shared_ptr<PNode> a;
			std::shared_ptr<PNode> b;

			for (size_t x{}; x < 10; x++)
			{
				for (size_t z{}; z < 10; z++)
				{
					glm::vec3 location = glm::vec3(Center.x + (/*GridSpacing **/ x) - (float)GridSize / 2, 0, Center.z + (/*GridSpacing **/ z) - (float)GridSize / 2);
					m_Nodes.emplace_back(std::make_shared<PNode>(GenerateNodeName(), location));
					m_NodeLocations.push_back(location);
				}
			}
			for (size_t i{1}; i < m_Nodes.size(); i++)
			{
				if (m_Nodes.size() < i + 1) { break; }

				a = m_Nodes[i - 1];
				b = m_Nodes[i];
				if (i%10!=0 || i==0) {
					a->AddConnectedNode(b);
					b->AddConnectedNode(a);
				}

				if (m_Nodes.size() < i + 10) { continue; }
				b = m_Nodes[(i-1) + 10];
				a->AddConnectedNode(b);
				b->AddConnectedNode(a);
			}
			E_TRACE("Created A* grid");
			E_TRACE("{0} Connections: {1}", m_Nodes[0]->m_name, m_Nodes[0]->mConnectedNodes.size());
			E_TRACE("{0} Connections: {1}", m_Nodes[10]->m_name, m_Nodes[10]->mConnectedNodes.size());
		}
	}
}