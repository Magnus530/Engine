#ifndef BASICPARTICLERENDERER_H
#define BASICPARTICLERENDERER_H

#include <memory>
//#include <QOpenGLFunctions_4_1_Core>
#include <glad/glad.h>

//#include <QMatrix4x4>
#include <glm/mat4x4.hpp>
#include "Engine/Renderer/vertex.h"

namespace particles
{

    class ParticleSystem;

    class IParticleRenderer /*:  protected QOpenGLFunctions_4_1_Core*/
    {
    public:
        IParticleRenderer() { }
        virtual ~IParticleRenderer() { }

        virtual void generate(ParticleSystem *sys, bool useQuads) = 0;
        virtual void destroy() = 0;
        virtual void update() = 0;
        virtual void render() = 0;
    };

    class RendererFactory
    {
    public:
        static std::shared_ptr<IParticleRenderer> create(const char *name);
    };

    class GLParticleRenderer : public IParticleRenderer
        {
        protected:
            ParticleSystem *m_system{ nullptr };

            unsigned int m_bufPos{ 0 };
            unsigned int m_bufCol{ 0 };
            unsigned int m_vao{ 0 };
//            GLuint m_vao{ 0 };
        public:
            GLParticleRenderer() { }
            ~GLParticleRenderer() { destroy(); }

            void generate(ParticleSystem *sys, bool useQuads) override;
            void destroy() override;
            void update() override;
            void render() override;

            glm::mat4 mMatrix;
            std::vector<Engine::Vertex> mVertices;
        };
}

#endif // BASICPARTICLERENDERER_H
