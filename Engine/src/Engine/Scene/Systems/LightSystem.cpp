#include "epch.h"
#include "LightSystem.h"

namespace Engine
{
	void LightSystem::UpdateLight(Engine::PhongMaterialComponent& pComp, Engine::Entity& lEntity, Engine::PerspectiveCameraController& pCam)
	{
		pComp.m_AmbientStrength = lEntity.GetComponent<Engine::LightComponent>().m_AmbientStrength;
		pComp.m_LightPosition = lEntity.GetComponent<Engine::TransformComponent>().GetLocation();
		pComp.m_PCamPosition = pCam.GetPos();
		pComp.m_LightColor = lEntity.GetComponent<Engine::LightComponent>().m_LightColor;
		pComp.m_SpecularStrength = lEntity.GetComponent<Engine::LightComponent>().m_SpecularStrength;
	}
}