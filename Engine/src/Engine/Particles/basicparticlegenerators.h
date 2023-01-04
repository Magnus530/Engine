#ifndef BASICPARTICLEGENERATORS_H
#define BASICPARTICLEGENERATORS_H

# define M_PI           3.14159265358979323846  /* pi */

#include <vector>
//#include <math.h>
#include "basicparticles.h"
namespace particles
{
    namespace generators
    {
        class BoxPosGen: public ParticleGenerator
        {
        public:
            glm::vec4 m_pos{0.0f};
            glm::vec4 m_maxStartPosOffset{2.0};
        public:
            BoxPosGen() { };

            virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;

        };

        class RoundPosGen : public ParticleGenerator
        {
        public:
            glm::vec4 m_center{ 0.0};
            float m_radX{ 0.0f};
            float m_radY{ 0.0f};
        public:
            RoundPosGen() { };
            RoundPosGen(const glm::vec4 &center, double radX, double radY)
                : m_center(center)
                , m_radX((float)radX)
                , m_radY((float)radY)
            { }

            virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
        };

        class BasicColorGen : public ParticleGenerator
        {
        public:
            glm::vec4 m_minStartCol{ 1.0, 1.0, 1.0, 1.0 };
            glm::vec4 m_maxStartCol{ 1.0, 0.0, 1.0, 1.0 };
            glm::vec4 m_minEndCol{ 0.7, 0.7, 1.0, 1.0 };
            glm::vec4 m_maxEndCol{ 0.9, 0.9, 1.0, 1.0 };
        public:
            BasicColorGen() { }

            virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
        };

        class BasicVelGen : public ParticleGenerator
        {
        public:
            glm::vec4 m_minStartVel{ -0.0, -1.0, -0.0, 0.0 };
            glm::vec4 m_maxStartVel{ 0.0, 0.0, 0.0, 0.0 };
        public:
            BasicVelGen() { }

            virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
        };

        class SphereVelGen : public ParticleGenerator
        {
        public:
            float m_minVel{ 0.0f };
            float m_maxVel{ 0.0f };
        public:
            SphereVelGen() { }

            virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
        };

        class BasicTimeGen : public ParticleGenerator
        {
        public:
            float m_minTime{ 5.0 };
            float m_maxTime{ 20.0 };
        public:
            BasicTimeGen() { }

            virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
        };
    }
}

#endif // BASICPARTICLEGENERATORS_H
