#pragma once

#include "glm/glm.hpp"

namespace Pathfinding {
	inline int floatConvert{ 10 };
	//typedef std::pair<PNode*, PLine*>& Path;

	struct PPath
	{
		PPath() {};
		PPath(class PNode* start, class PNode* target) { mStartNode = start; mTargetNode = target; }
		class PNode* mStartNode;
		class PNode* mTargetNode;
		std::vector<class PNode*> mNodes;	// Nodes on the path toward the target
		std::vector<class PNode*> mCheckedNodes;
		std::vector<struct PLine*> mLines;
		int Value{};

		void AddPath(class PNode* N, struct PLine* L, int v)
		{
			mNodes.push_back(N);
			mLines.push_back(L);
			Value += v;
		}
		bool ContainsNode(class PNode* N)
		{
			if (N == mStartNode) { return true; }
			for (const auto& it : mNodes) 
				if (it == N) { return true; } 
			return false;
		}
		void ClearPath()
		{
			mNodes.clear();
			mCheckedNodes.clear();
			mLines.clear();
		}
	};

	enum EPathSearch
	{
		Start,
		Search,
		Found
	};

	class ENGINE_API PNode
	{
	public:

		PNode();
		~PNode();

		std::vector<struct PLine*> mConnectedPaths;
		glm::vec3 mPosition;

		/* The length from this node to the target node */
		int GetInternalValue(const PNode* target);

		PNode* FindNextNode(EPathSearch& SearchMode, PPath* path, const PNode* Start, const PNode* Target);
	};

	struct PLine
	{
		PLine(){}
		PLine(class PNode* a, class PNode* b)
		{
			Node_A = a;
			Node_B = b;

			SetLength();
		}

		class PNode* Node_A{ nullptr };
		class PNode* Node_B{ nullptr };

		int Length{};

		void SetLength()
		{
			glm::vec3 v = Node_A->mPosition - Node_B->mPosition;
			Length = (int)v.length() * floatConvert;
		}

		PNode* GetOtherNode(const PNode* current) 
		{
			PNode* p;
			current == Node_A ? p = Node_A : p = Node_B;
			return p;
		}
	};

	/* First use of pathfinding - Node to Node */
	bool FindPath(PPath* Path, PNode* Start, PNode* Target);

	/* Second use of pathfinding - Position to Position */
	//void StartPathfinding(const glm::vec3& StartPos, const glm::vec3& TargetPos);

	
}