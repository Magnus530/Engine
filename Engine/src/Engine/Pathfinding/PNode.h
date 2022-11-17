#pragma once
//#include "Engine/Core.h"
#include "Engine/Core/Core.h"
#include "Engine.h"
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

		PNode();
		PNode(glm::vec3 position);
		~PNode();

		ENodeType NodeType = ENodeType::NT_None;

		glm::vec3 m_Position;
		std::vector<PNode*> mConnectedNodes;
		void AddConnectedNode(PNode* node) { mConnectedNodes.push_back(node); }
		/* Direct connection to next node when moving through the path from the end node */
		PNode* m_Connection{ nullptr };

		int F;	// Total DistanceValue
		int G;	// DistanceValue from StartNode to this
		int H;	// DistanceValue from EndNode to this


		int GetDistanceToNode(PNode* target);
		void InitValues(PNode* start, PNode* target);

		void SetG(int g) { G = g; F = G + H; }
		void SetH(PNode* target) { H = GetDistanceToNode(target); }

		bool IsBlock() const { return NodeType == ENodeType::NT_Block; }

	};

	std::vector<PNode*> FindPath(PNode* start, PNode* end);
	

	// Spawner noder for testing 
	int GridSize{ 1000 };
	float GridSpacing{ 100.f };
	glm::vec3 Center(0, 0, 0);
	std::vector<std::shared_ptr<PNode>> mNodes;
	void SpawnGrid()
	{
		std::shared_ptr<PNode> a;
		std::shared_ptr<PNode> b;

		for (size_t x{}; x < 10; x++)
		{
			for (size_t y{}; y < 10; y++)
			{
				glm::vec3 location = glm::vec3(Center.x + (GridSpacing * x) - (float)GridSize / 2, Center.y + (GridSpacing * y) - (float)GridSize / 2, Center.z);
				mNodes.emplace_back(std::make_shared<PNode>(location));
			}
		}
		for (size_t i{ 1 }; i < mNodes.size(); i++)
		{
			if (mNodes.size() < i + 1) break;

			a = mNodes[i - 1];
			b = mNodes[i];
			if (i % 10 != 0 || i == 0) {
				a->AddConnectedNode(b.get());
				b->AddConnectedNode(a.get());
			}

			if (mNodes.size() < i + 10) continue;
			b = mNodes[i - 1 + 10];
			a->AddConnectedNode(b.get());
			b->AddConnectedNode(a.get());
		}
	}
}