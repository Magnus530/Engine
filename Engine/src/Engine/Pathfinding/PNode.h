#pragma once
#include "glm/glm.hpp"

namespace Engine {
	inline float floatConvert{ 1000.f };

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
		std::string m_Name;
		glm::vec3 m_Position{};
		bool bIsObstruction{};
	};

	class PNode
	{
	public:
		PNode(glm::vec3 position)
		{
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
		std::shared_ptr<PathNodeNeighbors> m_Neighbors;
		std::shared_ptr<PNode> m_Connection;
		std::shared_ptr<PathNodeDistanceValues> m_DistanceValues;


		void AddConnectedNode(std::shared_ptr<PNode> node) { 
			m_Neighbors->neighbors.push_back(node);
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
		}
		void SetBlock(bool b)
		{
			m_Data->bIsObstruction = true;
		}
	};
}