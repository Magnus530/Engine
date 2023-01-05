#ifndef BASICPARTICLEMANAGER_H
#define BASICPARTICLEMANAGER_H


#include "basicparticles.h"
#include "basicparticlegenerators.h"
#include "basicparticlerenderer.h"
#include "Engine/Renderer/Camera.h"
namespace particles{
    class BasicParticleManager
    {
    public:
        explicit BasicParticleManager(size_t maxCount, glm::vec4 loc);
        void update(double dt);
    private:
        particles::GLParticleRenderer* p_Render;
        particles::ParticleSystem* p_System;
    };
}
#endif // BASICPARTICLEMANAGER_H
