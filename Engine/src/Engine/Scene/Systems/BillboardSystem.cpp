#include "epch.h"
#include "Engine/Scene/Systems/BillboardSystem.h"
#include "Engine/Scene/Systems/TransformSystem.h"

namespace Engine
{
	void BillboardSystem::UpdateBillboard(Engine::Entity& entity, Engine::PerspectiveCameraController& pCam)
	{
		Engine::TransformSystem::RotateToPosition(entity.GetComponent<Engine::TransformComponent>(), pCam.GetPos());
	}
}