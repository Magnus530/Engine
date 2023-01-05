#pragma once

#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"
namespace Engine
{
	class ParticleSystem
	{
	public:
		static void UpdateParticle(Engine::ParticleManagerComponent& pComp, double dt);
	};
}
