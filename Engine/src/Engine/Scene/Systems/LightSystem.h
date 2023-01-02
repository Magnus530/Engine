#pragma once

#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Renderer/PerspectiveCameraController.h"

namespace Engine
{
	class LightSystem
	{
	public:
		static void UpdateLight(Engine::PhongMaterialComponent& pComp, Engine::Entity& lEntity, Engine::PerspectiveCameraController& pCam);
	};
}