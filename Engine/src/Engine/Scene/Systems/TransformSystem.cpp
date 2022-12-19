#include "epch.h"
#include "TransformSystem.h"
#include <glm/gtx/transform.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

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
		glm::vec3 scale = GetScale(comp);
		SetScale(comp, 1.f);
		glm::mat4 rot(glm::mat3(comp.m_Transform));
		comp.m_Transform *= glm::inverse(rot);

		glm::quat MyQuat(rot);

		MyQuat = glm::angleAxis(radians, rotationAxis) * MyQuat;
		comp.m_Transform *= glm::toMat4(MyQuat);
		
		SetScale(comp, scale);
	}
	void TransformSystem::AddLocalRotation(TransformComponent& comp, float radians, glm::vec3& rotationAxis)
	{
		glm::vec3 scale = GetScale(comp);
		SetScale(comp, 1.f);
		comp.m_Transform = glm::rotate(comp.m_Transform, radians, rotationAxis);
		SetScale(comp, scale);
	}
	void TransformSystem::RotateToPosition(TransformComponent& comp, glm::vec3 position)
	{
		glm::vec3 direction = position - comp.GetPosition();
		RotateToDirectionVector(comp, direction);
	}
	void TransformSystem::RotateToPosition2D(TransformComponent& comp, glm::vec3 position)
	{
		glm::vec3 direction = position - comp.GetPosition();
		direction.y = 0.f;
		RotateToDirectionVector(comp, direction);
	}
	void TransformSystem::RotateToDirectionVector(TransformComponent& comp, glm::vec3 direction)
	{		
		// Find Orientation Quaternion
		direction = glm::normalize(direction);
		glm::quat rot1 = RotationBetweenVectors(glm::vec3(0, 0, 1), direction);
		glm::vec3 desiredUp(0, 1, 0);
		glm::vec3 right = glm::cross(direction, desiredUp);	// Get right vector from direction and desired up
		desiredUp = glm::cross(right, direction);
		
		glm::vec3 newUp = rot1 * glm::vec3(0, 1, 0);
		glm::quat rot2 = RotationBetweenVectors(newUp, desiredUp);

		glm::quat targetOrientation = rot2 * rot1;

		// Rotate Object to Orientation Quaternion
		glm::vec3 scale = GetScale(comp);
		SetScale(comp, 1.f);
		glm::mat4 rot(glm::mat3(comp.m_Transform));
		comp.m_Transform *= glm::inverse(rot);
		comp.m_Transform *= glm::toMat4(targetOrientation);
		SetScale(comp, scale);

	}
	void TransformSystem::RotateToDirectionVector2D(TransformComponent& comp, glm::vec3 direction)
	{
		direction.y = 0;
		RotateToDirectionVector(comp, direction);
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

	glm::quat TransformSystem::RotationBetweenVectors(glm::vec3 start, glm::vec3 destination)
	{
		start = normalize(start);
		destination = normalize(destination);

		float cosTheta = glm::dot(start, destination);
		glm::vec3 rotationAxis;

		rotationAxis = glm::cross(start, destination);

		if (cosTheta < -1 + 0.001f)
		{
			rotationAxis = cross(glm::vec3(0, 0, 1), start);
			if (glm::length2(rotationAxis) < 0.01)
				rotationAxis = glm::cross(glm::vec3(1, 0, 0), start);
			rotationAxis = glm::normalize(rotationAxis);
			return glm::angleAxis(glm::radians(180.f), rotationAxis);
		}

		float s = sqrt((1 + cosTheta) * 2);
		float invs = 1 / s;

		return glm::quat
		(
			s * 0.5f,
			rotationAxis.x * invs,
			rotationAxis.y * invs,
			rotationAxis.z * invs
		);
	}
}