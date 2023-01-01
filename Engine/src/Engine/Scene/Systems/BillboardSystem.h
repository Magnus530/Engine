#pragma once

#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Renderer/PerspectiveCameraController.h"

namespace Engine
{
	class BillboardSystem
	{
	public:
		static void UpdateBillboard(Engine::BillboardMaterialComponent& bComp, Engine::PerspectiveCameraController& pCam);
	};
}