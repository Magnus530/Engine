#include "epch.h"

#include <glad/glad.h>

#include "basicparticlemanager.h"

#include "Engine/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
namespace particles{
    BasicParticleManager::BasicParticleManager(size_t maxCount)
    {
        //  Force activate old OpenGL code that should be active by default
        glEnable(0x8861);

        p_System = new particles::ParticleSystem(maxCount);

        std::shared_ptr<particles::ParticleEmitter> pE( new particles::ParticleEmitter());
        p_System->addEmitter(pE);
        pE->m_emitRate = 1000.0f;
        pE->addGenerator(std::shared_ptr<particles::generators::BoxPosGen>(new particles::generators::BoxPosGen));
        pE->addGenerator(std::shared_ptr<particles::generators::BasicColorGen>(new particles::generators::BasicColorGen));
        pE->addGenerator(std::shared_ptr<particles::generators::BasicVelGen>(new particles::generators::BasicVelGen));
        pE->addGenerator(std::shared_ptr<particles::generators::BasicTimeGen>(new particles::generators::BasicTimeGen));

        p_System->addUpater(std::shared_ptr<particles::updaters::EulerUpdater>(new particles::updaters::EulerUpdater));
        p_System->addUpater(std::shared_ptr<particles::updaters::BasicColorUpdater>(new particles::updaters::BasicColorUpdater));
        p_System->addUpater(std::shared_ptr<particles::updaters::BasicTimeUpdater>(new particles::updaters::BasicTimeUpdater));

        p_Render = new particles::GLParticleRenderer;
        p_Render->generate(p_System, false);
    }

    void BasicParticleManager::update(double dt, Engine::Camera& camera)
    {
        p_System->update(dt);
        p_Render->update();
        p_Render->render();
    }
}
