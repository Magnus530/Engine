#include "epch.h"

#include <glad/glad.h>

#include "basicparticlemanager.h"

#include "Engine/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
namespace particles{
    BasicParticleManager::BasicParticleManager(size_t maxCount, glm::vec4 loc)
    {
        //  Force activate old OpenGL code that should be active by default
        glEnable(0x8861);

        p_System = new particles::ParticleSystem(maxCount);

        std::shared_ptr<particles::ParticleEmitter> pE( new particles::ParticleEmitter());
        p_System->addEmitter(pE);
        pE->m_emitRate = 100.0f;
        //pE->addGenerator(std::shared_ptr<particles::generators::BoxPosGen>(new particles::generators::BoxPosGen));
        std::shared_ptr<particles::generators::BoxPosGen> BoxGen(new particles::generators::BoxPosGen);
        pE->addGenerator(BoxGen);
        //BoxGen->m_pos = glm::vec4{ -3.0f };
        BoxGen->m_pos = loc;
        BoxGen->m_maxStartPosOffset = glm::vec4{ 20.0f, 1.0f, 20.0f, 1.0f };
        //pE->addGenerator(std::shared_ptr<particles::generators::BasicColorGen>(new particles::generators::BasicColorGen));
        std::shared_ptr<particles::generators::BasicColorGen> ColorGen(new particles::generators::BasicColorGen);
        pE->addGenerator(ColorGen);
        ColorGen->m_minStartCol = glm::vec4{ 1.0f };
        ColorGen->m_maxStartCol = glm::vec4{1.0, 0.0, 1.0, 1.0 };
        ColorGen->m_minEndCol = glm::vec4{ 0.7, 0.7, 1.0, 1.0 };
        ColorGen->m_maxEndCol = glm::vec4{ 0.9, 0.9, 1.0, 1.0 };
        //pE->addGenerator(std::shared_ptr<particles::generators::BasicVelGen>(new particles::generators::BasicVelGen));
        std::shared_ptr<particles::generators::BasicVelGen> VelGen(new particles::generators::BasicVelGen);
        pE->addGenerator(VelGen);
        VelGen->m_minStartVel = glm::vec4{ -0.0, -0.5, -0.0, 0.0 };
        VelGen->m_maxStartVel = glm::vec4{ -0.0, -0.7, -0.0, 0.0 };
        //pE->addGenerator(std::shared_ptr<particles::generators::BasicTimeGen>(new particles::generators::BasicTimeGen));
        std::shared_ptr<particles::generators::BasicTimeGen> TimeGen(new particles::generators::BasicTimeGen);
        pE->addGenerator(TimeGen);
        TimeGen->m_minTime = 5.0f;
        TimeGen->m_maxTime = 7.0f;


        //p_System->addUpater(std::shared_ptr<particles::updaters::EulerUpdater>(new particles::updaters::EulerUpdater));
        std::shared_ptr<particles::updaters::EulerUpdater> EulerUp(new particles::updaters::EulerUpdater);
        p_System->addUpater(EulerUp);
        EulerUp->m_globalAcceleration = glm::vec4{ 0.5f, 0.0f, 0.5f, 0.0f };
        //p_System->addUpater(std::shared_ptr<particles::updaters::BasicColorUpdater>(new particles::updaters::BasicColorUpdater));
        std::shared_ptr<particles::updaters::BasicColorUpdater> ColorUp(new particles::updaters::BasicColorUpdater);
        p_System->addUpater(ColorUp);
        //p_System->addUpater(std::shared_ptr<particles::updaters::BasicTimeUpdater>(new particles::updaters::BasicTimeUpdater));
        std::shared_ptr<particles::updaters::BasicTimeUpdater> TimeUp(new particles::updaters::BasicTimeUpdater);
        p_System->addUpater(TimeUp);

        p_Render = new particles::GLParticleRenderer;
        p_Render->generate(p_System, false);
    }

    void BasicParticleManager::update(double dt)
    {
        p_System->update(dt);
        p_Render->update();
        p_Render->render();
    }
}
