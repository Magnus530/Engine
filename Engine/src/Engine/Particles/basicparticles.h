#ifndef BASICPARTICLES_H
#define BASICPARTICLES_H

#include <vector>
#include <memory>
#include <glm/vec4.hpp>

/***************************************************************************************
*   This code was made follwing a guide called "Three Particel Effects"
*    Title: <BasicParticles.hpp>    
*           <ParticleUpdaters.hpp>
*    Author: Bartlomiej Filipek>
*    Date:  <Apr 27, 2014>
*           <Jun  5, 2014>
*    Availability: <https://gist.github.com/fenbfd>
*
***************************************************************************************/

namespace particles{
    class ParticleData
    {
    public:
        std::unique_ptr<glm::vec4[]> m_pos;
        std::unique_ptr<glm::vec4[]> m_col;
        std::unique_ptr<glm::vec4[]> m_startCol;
        std::unique_ptr<glm::vec4[]> m_endCol;
        std::unique_ptr<glm::vec4[]> m_vel;
        std::unique_ptr<glm::vec4[]> m_acc;
        std::unique_ptr<glm::vec4[]> m_time;
        std::unique_ptr<bool[]>  m_alive;

        size_t m_count{ 0 };
        size_t m_countAlive{ 0 };

    public:
        ParticleData() { }
        explicit ParticleData(size_t maxCount) { generate(maxCount); }
        ~ParticleData() { }

        ParticleData(const ParticleData &) = delete;
        ParticleData &operator=(const ParticleData &) = delete;

        void generate(size_t maxSize);
        void kill(size_t id);
        void wake(size_t id);
        void swapData(size_t a, size_t b);
    };

//******************************************************************************************************************************************************************************************************
//  ParticleGenerator:
//  Needs:
//      Data
//  See own file
//**********************
    class ParticleGenerator
    {
    public:
        ParticleGenerator() { }
        virtual ~ParticleGenerator() { }

        virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) =0;
    };

//******************************************************************************************************************************************************************************************************
//  ParticleEmitter:
//  Needs:
//      Generator
//**********************
    class ParticleEmitter
    {
    protected:
        std::vector<std::shared_ptr<ParticleGenerator>> m_generators;
    public:
//        float m_emitRate{ 0.0 };
        float m_emitRate{1000.0f};
    public:
        ParticleEmitter() { }
        virtual ~ParticleEmitter() { };

        virtual void emitt(double dt, ParticleData *p);

        void addGenerator(std::shared_ptr<ParticleGenerator> gen) {m_generators.push_back(gen);}
    };

//******************************************************************************************************************************************************************************************************
//  ParticleUpdaters:
//  Needs:
//      Data
//**********************
    class ParticleUpdater
    {
    public:
        ParticleUpdater() { }
        virtual ~ParticleUpdater() { }

        virtual void update(double dt, ParticleData *p) = 0;
    };

    namespace updaters
    {
        class EulerUpdater : public ParticleUpdater
        {
        public:
//            glm::vec4 m_globalAcceleration{ 0.0f };
            glm::vec4 m_globalAcceleration{ 0.0f };
        public:
            virtual void update(double dt, ParticleData *p) override;
        };

        // collision with the floor :) todo: implement a collision model
        class FloorUpdater : public particles::ParticleUpdater
        {
        public:
//            float m_floorY{ 0.0f };
//            float m_bounceFactor{ 0.5f };
            float m_floorY{ 0.0f };
            float m_bounceFactor{ 0.5f };
        public:
            virtual void update(double dt, ParticleData *p) override;
        };

        class AttractorUpdater : public particles::ParticleUpdater
        {
        protected:
            std::vector<glm::vec4> m_attractors; // .w is force
        public:
            virtual void update(double dt, ParticleData *p) override;

            size_t collectionSize() const { return m_attractors.size(); }
            void add(const glm::vec4 &attr) { m_attractors.push_back(attr); }
            glm::vec4 &get(size_t id) { return m_attractors[id]; }
        };

        class BasicColorUpdater : public ParticleUpdater
        {
        public:
            virtual void update(double dt, ParticleData *p) override;
        };

        class PosColorUpdater : public ParticleUpdater
        {
        public:
//            glm::vec4 m_minPos{ 0.0 };
//            glm::vec4 m_maxPos{ 1.0 };
            glm::vec4 m_minPos{ 0.0 };
            glm::vec4 m_maxPos{ 1.0 };
        public:
            virtual void update(double dt, ParticleData *p) override;
        };

        class VelColorUpdater : public ParticleUpdater
        {
        public:
//            glm::vec4 m_minVel{ 0.0 };
//            glm::vec4 m_maxVel{ 1.0 };
            glm::vec4 m_minVel{ 0.0 };
            glm::vec4 m_maxVel{ 1.0 };
        public:
            virtual void update(double dt, ParticleData *p) override;
        };

        class BasicTimeUpdater : public ParticleUpdater
        {
        public:
            virtual void update(double dt, ParticleData *p) override;
        };
    }

//******************************************************************************************************************************************************************************************************
//  ParticleSystem:
//  Needs:
//      Emitter
//      Updater
//**********************
    class ParticleSystem
    {
    protected:
        ParticleData m_particles;

        size_t m_count;

        std::vector<std::shared_ptr<ParticleEmitter>> m_emitters;
        std::vector<std::shared_ptr<ParticleUpdater>> m_updaters;

    public:
        explicit ParticleSystem(size_t maxCount);
        virtual ~ParticleSystem() { }

        ParticleSystem(const ParticleSystem &) = delete;
        ParticleSystem &operator=(const ParticleSystem &)= delete;

        virtual void update(double dt);
        virtual void reset();

        virtual size_t numAllParticles() const {return m_particles.m_count;}
        virtual size_t numAliveParticles() const {return m_particles.m_countAlive;}

        void addEmitter(std::shared_ptr<ParticleEmitter> em) { m_emitters.push_back(em); }
        void addUpater(std::shared_ptr<ParticleUpdater> up) { m_updaters.push_back(up); }

        ParticleData *finalData() { return &m_particles;}
    };
}
#endif // BASICPARTICLES_H
