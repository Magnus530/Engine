#include "epch.h"
#include "Engine/Scene/Systems/BillboardSystem.h"

namespace Engine
{
	void BillboardSystem::UpdateBillboard(Engine::BillboardMaterialComponent& bComp, Engine::PerspectiveCameraController& pCam)
	{
		bComp.m_CamRight = pCam.GetCamera().right();
		bComp.m_CamUp = pCam.GetCamera().up();
		bComp.m_ProjViewMat = pCam.GetCamera().GetProjectionViewMatrix();


		// ta inn entity og se på camera med glm::lookat (entitypos, entitypos + entitydir, upvector til entity (0, 1, 0)) wrap matrise i glm::inverse gir en tansform
		// matrix som erstatter transform i componenten.
		// kamerapos - entitypos = glm::normalize(billboard rotation)

		//lookat camera
	}
}