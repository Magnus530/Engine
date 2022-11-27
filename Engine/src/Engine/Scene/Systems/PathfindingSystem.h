#pragma once
#include <Engine.h>
#include "../Components.h"
#include "../../Pathfinding/PNode.h"

namespace Engine {

	class PathfindingSystem
	{
	public:	// Functions
		static void FindPath(PathfindingComponent& comp, std::shared_ptr<PNode> start, std::shared_ptr<PNode> end);
		static void MoveAlongPath(PathfindingComponent& pathfinder, TransformComponent& transform, float deltatime);

	private:	// Data
		static std::vector<std::shared_ptr<PNode>> FindPath(std::shared_ptr<PNode> start, std::shared_ptr<PNode> end);

		static glm::vec3 GetPositionAlongSpline(std::vector<glm::vec3>& splinepoints, float t);
		
	};
}