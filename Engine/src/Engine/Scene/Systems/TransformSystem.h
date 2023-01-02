#pragma once
//#include <Engine.h>

#include "../Components.h"

namespace Engine {

	class TransformSystem
	{
	public:
		static void ResetTransforms(TransformComponent& comp);

		static void SetWorldPosition(TransformComponent& comp, glm::vec3 position);
		static void AddWorldPosition(TransformComponent& comp, glm::vec3 addPosition);

		static void AddWorldRotation(TransformComponent& comp, float radians, glm::vec3& rotationAxis);
		static void AddLocalRotation(TransformComponent& comp, float radians, glm::vec3& rotationAxis);

		static glm::vec3 GetForwardVector(const TransformComponent& comp);
		static glm::vec3 GetUpVector(const TransformComponent& comp);
		static glm::vec3 GetRightVector(const TransformComponent& comp);

		static void SetRotation(TransformComponent& comp, glm::vec3 Rotator);

		static void RotateToPosition(TransformComponent& comp, glm::vec3 position);
		static void RotateToPosition2D(TransformComponent& comp, glm::vec3 position);
		static void RotateToDirectionVector(TransformComponent& comp, glm::vec3 direction);
		static void RotateToDirectionVector2D(TransformComponent& comp, glm::vec3 direction);

		static glm::vec3 GetScale(const TransformComponent& comp);
		static void SetScale(TransformComponent& comp, float scale);
		static void SetScale(TransformComponent& comp, glm::vec3 scale);
	
	private:
		static glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 destination);
	};
}