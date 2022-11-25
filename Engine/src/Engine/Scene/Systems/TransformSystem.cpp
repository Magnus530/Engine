#include "epch.h"
#include "TransformSystem.h"
#include <glm/gtx/transform.hpp>

namespace Engine {
	void TransformSystem::UpdateMatrix(TransformComponent& comp)
	{
		comp.m_Transform = comp.m_Scale * comp.m_Rotation * comp.m_Position;
	}

	void TransformSystem::SetWorldPosition(TransformComponent& comp, glm::vec3 position)
	{
		comp.m_Position = glm::translate(glm::inverse(comp.m_Rotation) * glm::mat4(1.f), position) * comp.m_Rotation;
	}
	void TransformSystem::AddWorldPosition(TransformComponent& comp, glm::vec3 addPosition)
	{
		glm::vec3 currentPosition(comp.m_Transform[3]);
		glm::vec3 travel = addPosition - currentPosition;
		comp.m_Position = glm::translate(glm::inverse(comp.m_Rotation) * comp.m_Position, travel) * comp.m_Rotation;
	}
	void TransformSystem::AddWorldRotation(TransformComponent& comp, float radians, glm::vec3& rotationAxis)
	{
		comp.m_Rotation *= glm::inverse(comp.m_Rotation) * glm::rotate(glm::mat4(1.f), radians, rotationAxis) * comp.m_Rotation;
	}
	void TransformSystem::AddLocalRotation(TransformComponent& comp, float radians, glm::vec3& rotationAxis)
	{
		comp.m_Rotation = glm::rotate(comp.m_Rotation, radians, rotationAxis);
	}
	//void TransformSystem::AddScale(TransformComponent& comp, float scale, glm::vec3 scaleAxis)
	//{
	//	comp.m_Scale = glm::scale(comp.m_Scale, glm::vec3(1.f) + (scaleAxis * scale));
	//	UpdateMatrix(comp);
	//}
}