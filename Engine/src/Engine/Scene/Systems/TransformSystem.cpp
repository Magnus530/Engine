#include "epch.h"
#include "TransformSystem.h"
#include <glm/gtx/transform.hpp>

namespace Engine {
	void TransformSystem::SetWorldPosition(TransformComponent& comp, glm::vec3 position)
	{
		glm::mat4 rot(glm::mat3(comp.m_Transform));
		glm::mat4 pos = glm::translate(glm::inverse(rot) * glm::mat4(1.f), position) * rot;
		comp.m_Transform = rot * pos;
	}
	void TransformSystem::AddWorldPosition(TransformComponent& comp, glm::vec3 addPosition)
	{
		glm::vec3 currentPosition(comp.m_Transform[3]);
		glm::vec3 travel = addPosition - currentPosition;
		comp.m_Transform = glm::translate(comp.m_Transform, addPosition);
	}
	void TransformSystem::AddWorldRotation(TransformComponent& comp, float radians, glm::vec3& rotationAxis)
	{
		glm::mat4 rot(glm::mat3(comp.m_Transform));
		comp.m_Transform *= glm::inverse(rot) * glm::rotate(glm::mat4(1.f), radians, rotationAxis) * rot;
	}
	void TransformSystem::AddLocalRotation(TransformComponent& comp, float radians, glm::vec3& rotationAxis)
	{
		glm::vec3 scale = GetScale(comp);
		SetScale(comp, 1.f);
		comp.m_Transform= glm::rotate(comp.m_Transform, radians, rotationAxis);
		SetScale(comp, scale);
	}
	void TransformSystem::RotateToPosition(TransformComponent& comp, glm::vec3 position)
	{
		glm::vec3 direction = position - comp.GetPosition();
		RotateToDirectionVector(comp, direction);
	}
	void TransformSystem::RotateToDirectionVector(TransformComponent& comp, glm::vec3 direction)
	{
		glm::vec3 v = glm::normalize(direction);
		glm::vec3 forward( 0,  0,  1);
		glm::vec3 right  ( 1,  0,  0);

		float dotForward = glm::dot(v, forward);
		float dotRight = glm::dot(v, right);

		float angle = acosf(dotForward);
		if (dotRight < 0.f) 
			angle *= -1.f;

		glm::mat4 rot(glm::mat3(comp.m_Transform));
		comp.m_Transform *= glm::inverse(rot) * glm::rotate(glm::mat4(1.f), angle, glm::vec3(0, 1, 0));
	}
	glm::vec3 TransformSystem::GetScale(const TransformComponent& comp)
	{
		float X = glm::length(comp.m_Transform[0]);
		float Y = glm::length(comp.m_Transform[1]);
		float Z = glm::length(comp.m_Transform[2]);
		return glm::vec3(X, Y, Z);
	}
	void TransformSystem::SetScale(TransformComponent& comp, float scale)
	{
		SetScale(comp, glm::vec3(scale));
	}
	void TransformSystem::SetScale(TransformComponent& comp, glm::vec3 scale)
	{
		if (scale.x <= 0.f) scale.x = (float)1E-5f;
		if (scale.y <= 0.f) scale.y = (float)1E-5f;
		if (scale.z <= 0.f) scale.z = (float)1E-5f;

		// Extract Scale
		glm::vec4& X = comp.m_Transform[0];
		glm::vec4& Y = comp.m_Transform[1];
		glm::vec4& Z = comp.m_Transform[2];

		// Reset Scale to 1
		X /= glm::length(X);
		Y /= glm::length(Y);
		Z /= glm::length(Z);

		// Set new Scale
		comp.m_Transform = glm::scale(comp.m_Transform, scale);
	}
	//glm::mat4 TransformSystem::ExtractRotation(const glm::mat4 transform)
	//{
	//	glm::mat4 rot{ glm::mat3(transform) };
	//	glm::vec4& X = rot[0];
	//	glm::vec4& Y = rot[1];
	//	glm::vec4& Z = rot[2];
	//	
	//	X /= glm::length(X);
	//	Y /= glm::length(Y);
	//	Z /= glm::length(Z);
	//
	//	return rot;
	//}
}