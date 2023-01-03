#ifndef BASICPARTICLEMANAGER_H
#define BASICPARTICLEMANAGER_H


#include "basicparticles.h"
#include "basicparticlegenerators.h"
#include "basicparticlerenderer.h"
#include "Engine/Renderer/Camera.h"
class Shader;
class Texture;
namespace particles{
    class BasicParticleManager
    {
    public:
        explicit BasicParticleManager(size_t maxCount);
        void update(double dt, Engine::Camera& camera);
    private:
        particles::GLParticleRenderer* p_Render;
        particles::ParticleSystem* p_System;

        Shader *mShaderProgram{nullptr};
        Texture* mTexture{ nullptr };

        GLint mMatrixUniform{-1};
        GLint vMatrixUniform{-1};
        GLint pMatrixUniform{-1};
        GLint mTextureUniform{-1};
    };
}
#endif // BASICPARTICLEMANAGER_H
