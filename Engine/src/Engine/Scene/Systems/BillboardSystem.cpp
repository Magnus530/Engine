#include "epch.h"
#include "Engine/Scene/Systems/BillboardSystem.h"
#include "Engine/Scene/Systems/TransformSystem.h"

namespace Engine
{
	void BillboardSystem::UpdateBillboard(Engine::Entity& entity, Engine::PerspectiveCameraController& pCam)
	{
		auto& tempTransform = entity.GetComponent<Engine::TransformComponent>();
		Engine::TransformSystem::RotateToDirectionVector(tempTransform, pCam.GetPos());
	}
}