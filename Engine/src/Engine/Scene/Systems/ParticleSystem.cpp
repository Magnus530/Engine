#include "epch.h"
#include "ParticleSystem.h"

void Engine::ParticleSystem::UpdateParticle(Engine::ParticleManagerComponent& pComp, double dt)
{
	pComp.m_ParticleManager->update(dt);
}
