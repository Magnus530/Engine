#pragma once
//#include <Engine.h>

#include "../Components.h"

namespace Engine {

	class TransformSystem
	{
	public:
		static void SetWorldPosition(TransformComponent& comp, glm::vec3 position);
		static void AddWorldPosition(TransformComponent& comp, glm::vec3 addPosition);

		static void AddWorldRotation(TransformComponent& comp, float radians, glm::vec3& rotationAxis);
		static void AddLocalRotation(TransformComponent& comp, float radians, glm::vec3& rotationAxis);

		static void RotateToVector(TransformComponent& comp, glm::vec3 vector);

		static glm::vec3 GetScale(const TransformComponent& comp);
		static void SetScale(TransformComponent& comp, float scale);
		static void SetScale(TransformComponent& comp, glm::vec3 scale);
	
	private:
		//static glm::mat4 ExtractRotation(const glm::mat4 transform);
	};
}