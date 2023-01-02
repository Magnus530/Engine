#include "epch.h"
#include "Engine/Scene/Systems/BillboardSystem.h"
#include "Engine/Scene/Systems/TransformSystem.h"

namespace Engine
{
	void BillboardSystem::UpdateBillboard(Engine::Entity& entity, Engine::PerspectiveCameraController& pCam)
	{
		auto& tempTransform = entity.GetComponent<Engine::TransformComponent>();

		glm::vec3 forward = glm::normalize(pCam.GetPos() - tempTransform.GetPosition());

		glm::mat4 tempMat = glm::inverse(glm::lookAt(tempTransform.GetPosition(), tempTransform.GetPosition() - forward, glm::vec3{0.f, 1.f, 0.f}));
		
		tempTransform.m_Transform = tempMat;

		//Engine::TransformSystem::RotateToPosition(tempTransform, pCam.GetPos());
		
	}
}