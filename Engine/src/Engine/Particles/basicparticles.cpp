#include "epch.h"

#include "basicparticles.h"

#include <assert.h>
#include <algorithm>
#include <glm/glm.hpp>

namespace particles
{
//******************************************************************************************************************************************************************************************************
//  ParticleData:
//**********************
    void ParticleData::generate(size_t maxSize)
    {
        m_count = maxSize;
        m_countAlive = 0;

        m_pos.reset(new glm::vec4[maxSize]);
        m_col.reset(new glm::vec4[maxSize]);
        m_startCol.reset(new glm::vec4[maxSize]);
        m_endCol.reset(new glm::vec4[maxSize]);
        m_vel.reset(new glm::vec4[maxSize]);
        m_acc.reset(new glm::vec4[maxSize]);
        m_time.reset(new glm::vec4[maxSize]);
        m_alive.reset(new bool[maxSize]);
//        for (size_t i = 0; i < maxSize; ++i)
//            m_col[i] = glm::vec4(0.0, 1.0, 1.0, 0.0);
    }

    void ParticleData::kill(size_t id)
    {
        if (m_countAlive > 0)
        {
            m_alive[id] = false;
            swapData(id, m_countAlive -1);
            m_countAlive--;
        }
    }

    void ParticleData::wake(size_t id)
    {
        if (m_countAlive < m_count)
        {
            m_alive[id] = true;
            swapData(id, m_countAlive);
            m_countAlive++;
        }
    }

    void ParticleData::swapData(size_t a, size_t b)
    {
        std::swap(m_pos[a], m_pos[b]);
        std::swap(m_col[a], m_col[b]);
        std::swap(m_startCol[a], m_startCol[b]);
        std::swap(m_endCol[a], m_endCol[b]);
        std::swap(m_vel[a], m_vel[b]);
        std::swap(m_acc[a], m_acc[b]);
        std::swap(m_time[a], m_time[b]);
        std::swap(m_alive[a], m_alive[b]);
    }

//******************************************************************************************************************************************************************************************************
//  ParticleEmitter:
//**********************
    void ParticleEmitter::emitt(double dt, ParticleData *p)
    {
        const size_t maxNewParticles = static_cast<size_t>(dt*m_emitRate);
        const size_t startId = p->m_countAlive;
        const size_t endId = std::min(startId + maxNewParticles, p->m_count-1);

        for(auto &gen :m_generators)
            gen->generate(dt, p, startId, endId);

        for(size_t i = startId; i< endId; ++i)
            p->wake(i);
    }


//******************************************************************************************************************************************************************************************************
//  ParticleUpdater:
//**********************
    namespace updaters
    {
        void EulerUpdater::update(double dt, ParticleData *p)
        {
            const glm::vec4 globalA{ dt * m_globalAcceleration.x, dt * m_globalAcceleration.y, dt * m_globalAcceleration.z, 0.0 };
            const float localDT = (float)dt;

            const unsigned int endId = p->m_countAlive;
            for (size_t i = 0; i < endId; ++i)
                p->m_acc[i] += globalA;

            for (size_t i = 0; i < endId; ++i)
                p->m_vel[i] += localDT * p->m_acc[i];

            for (size_t i = 0; i < endId; ++i)
                p->m_pos[i] += localDT * p->m_vel[i];
        }

        void FloorUpdater::update(double dt, ParticleData *p)
        {
            const float localDT = (float)dt;

            const size_t endId = p->m_countAlive;
            for (size_t i = 0; i < endId; ++i)
            {
                if (p->m_pos[i].y < m_floorY)
                {
                    glm::vec4 force = p->m_acc[i];
                    float normalFactor = glm::dot(force, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
                    if (normalFactor < 0.0f)
                        force -= glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) * normalFactor;

                    float velFactor = glm::dot(p->m_vel[i], glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
                    //if (velFactor < 0.0)
                    p->m_vel[i] -= glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) * (1.0f + m_bounceFactor) * velFactor;

                    p->m_acc[i] = force;
                }
            }

        }

        void AttractorUpdater::update(double dt, ParticleData *p)
        {
            const float localDT = (float)dt;

            const size_t endId = p->m_countAlive;
            const size_t countAttractors = m_attractors.size();
            glm::vec4 off;
            float dist;
            size_t a;
            for (size_t i = 0; i < endId; ++i)
            {
                for (a = 0; a < countAttractors; ++a)
                {
                    off.x = m_attractors[a].x - p->m_pos[i].x;
                    off.y = m_attractors[a].y - p->m_pos[i].y;
                    off.z = m_attractors[a].z - p->m_pos[i].z;
                    dist = glm::dot(off, off);

                    //if (fabs(dist) > 0.00001)
                    dist = m_attractors[a].w / dist;

                    p->m_acc[i] += off * dist;
                }
            }
        }

        void BasicColorUpdater::update(double dt, ParticleData *p)
        {
            const size_t endId = p->m_countAlive;
            for (size_t i = 0; i < endId; ++i)
                p->m_col[i] = glm::mix(p->m_startCol[i], p->m_endCol[i], p->m_time[i].z);
        }

        void PosColorUpdater::update(double dt, ParticleData *p)
        {
            const size_t endId = p->m_countAlive;
            float scaler, scaleg, scaleb;
            float diffr = m_maxPos.x - m_minPos.x;
            float diffg = m_maxPos.y - m_minPos.y;
            float diffb = m_maxPos.z - m_minPos.z;
            for (size_t i = 0; i < endId; ++i)
            {
                scaler = (p->m_pos[i].x - m_minPos.x) / diffr;
                scaleg = (p->m_pos[i].y - m_minPos.y) / diffg;
                scaleb = (p->m_pos[i].z - m_minPos.z) / diffb;
                p->m_col[i].r = scaler;// glm::mix(p->m_startCol[i].r, p->m_endCol[i].r, scaler);
                p->m_col[i].g = scaleg;// glm::mix(p->m_startCol[i].g, p->m_endCol[i].g, scaleg);
                p->m_col[i].b = scaleb;// glm::mix(p->m_startCol[i].b, p->m_endCol[i].b, scaleb);
                p->m_col[i].a = glm::mix(p->m_startCol[i].a, p->m_endCol[i].a, p->m_time[i].z);
            }
        }

        void VelColorUpdater::update(double dt, ParticleData *p)
        {
            const size_t endId = p->m_countAlive;
            float scaler, scaleg, scaleb;
            float diffr = m_maxVel.x - m_minVel.x;
            float diffg = m_maxVel.y - m_minVel.y;
            float diffb = m_maxVel.z - m_minVel.z;
            for (size_t i = 0; i < endId; ++i)
            {
                scaler = (p->m_vel[i].x - m_minVel.x) / diffr;
                scaleg = (p->m_vel[i].y - m_minVel.y) / diffg;
                scaleb = (p->m_vel[i].z - m_minVel.z) / diffb;
                p->m_col[i].r = scaler;// glm::mix(p->m_startCol[i].r, p->m_endCol[i].r, scaler);
                p->m_col[i].g = scaleg;// glm::mix(p->m_startCol[i].g, p->m_endCol[i].g, scaleg);
                p->m_col[i].b = scaleb;// glm::mix(p->m_startCol[i].b, p->m_endCol[i].b, scaleb);
                p->m_col[i].a = glm::mix(p->m_startCol[i].a, p->m_endCol[i].a, p->m_time[i].z);
            }
        }

        void BasicTimeUpdater::update(double dt, ParticleData *p)
        {
            unsigned int endId = p->m_countAlive;
            const float localDT = (float)dt;

            if (endId == 0) return;

            for (size_t i = 0; i < endId; ++i)
            {
                p->m_time[i].x -= localDT;
                // interpolation: from 0 (start of life) till 1 (end of life)
                p->m_time[i].z = (float)1.0 - (p->m_time[i].x*p->m_time[i].w); // .w is 1.0/max life time

                if (p->m_time[i].x < (float)0.0)
                {
                    p->kill(i);
                    endId = p->m_countAlive < p->m_count ? p->m_countAlive : p->m_count;
                }
            }
        }
    }

//******************************************************************************************************************************************************************************************************
//  ParticleSystem:
//**********************

    ParticleSystem::ParticleSystem(size_t maxCount)
    {
        m_count = maxCount;
        m_particles.generate(maxCount);
        m_aliveParticles.generate(maxCount);

        for(size_t i = 0; i < maxCount; ++i)
            m_particles.m_alive[i] = false;
    }

    void ParticleSystem::update(double dt)
    {
        for(auto & em: m_emitters)
            em->emitt(dt, &m_particles);
        for(size_t i = 0; i < m_count; ++i)
            m_particles.m_acc[i]=glm::vec4(0.0f);
        for(auto & up : m_updaters)
            up->update(dt, &m_particles);
    }

    void ParticleSystem::reset()
    {
        m_particles.m_countAlive = 0;
    }

    size_t ParticleSystem::computeMemoryUsage(const ParticleSystem &p)
    {
//        return 2 * ParticleData::computeMemoryUsage(p.m_particles);
        return 0;
    }
}
