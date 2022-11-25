#pragma once
//#include <Engine.h>

#include "../Components.h"

namespace Engine {

	class TransformSystem
	{
	public:
		static void UpdateMatrix(TransformComponent& comp);

		static void SetWorldPosition(TransformComponent& comp, glm::vec3 position);
		static void AddWorldPosition(TransformComponent& comp, glm::vec3 addPosition);

		static void AddWorldRotation(TransformComponent& comp, float radians, glm::vec3& rotationAxis);
		static void AddLocalRotation(TransformComponent& comp, float radians, glm::vec3& rotationAxis);

		
		//static void AddScale(TransformComponent& comp, float scale, glm::vec3 scaleAxis);
	};
}