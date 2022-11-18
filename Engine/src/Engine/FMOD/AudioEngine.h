#pragma once
/*
 * Code is used from https://www.codyclaborn.me/tutorials/making-a-basic-fmod-audio-engine-in-c/
 * which is referring to this \/
 * Code is used from Guy Somberg's "Game Audio Programming: Principles and practices"
 * ISBN-13: 978-1-4987-4673-1
 */

#include "fmod_studio.hpp"
#include "fmod.hpp"
#include "fmod_errors.h"
#include <glm/glm.hpp>
#include <string>
#include <map>
#include <math.h> // to changeoctave & changesemitone
#include <iostream>



namespace Engine {

    //#define FMOD_VERSION 0x00020209

    struct Implementation {
        Implementation();
        ~Implementation();

        void update(float fTimeDeltaSeconds);


        FMOD::Studio::System* mpStudioSystem;
        FMOD::System* mpSystem;

        int mnNextChannelId{};

        typedef std::map<std::string, FMOD::Sound*> SoundMap;
        typedef std::map<int, FMOD::Channel*> ChannelMap;
        typedef std::map<std::string, FMOD::Studio::EventInstance*> EventMap;
        typedef std::map<std::string, FMOD::Studio::Bank*> BankMap;

        BankMap mBanks;
        EventMap mEvents;
        SoundMap mSounds;
        ChannelMap mChannels;
        /*
        struct Channel
        {
            Channel(Implementation& tImplementation, int nSoundId, const AudioEngine::SoundDefinition& tSoundDefinition,
                    const glm::vec3& vPosition, float fVolumedB);
            enum class State
            { INITIALIZE, TOPLAY, LOADING, PLAYING, STOPPING, STOPPED, VIRTUALIZING, VIRUTAL, DEVIRTUALIZE, };
        Implementation& mImplementation;
        FMOD::Channel* mpChannel = nullptr;
        int mSoundId;
        glm::vec3 mvPosition;
        float mfVolumedB = 0.0f;
        float mfSoundVolume = 0.0f;
        State meState = State::INITIALIZE;
        bool mbStopRequested = false;
        //AudioFader mStopFader;
        //AudioFader mVirtualizeFader;
        void update(float fTimeDeltaSeconds);
        void updateChannelParameters();
        bool shouldBeVirtual(bool bAllowOneShotVirtuals) const;
        bool isPlaying() const;
        float getVolumedB() const;
    };
    */
    };

    class AudioEngine
    {
    public:

        // interface code
        static void init();
        static void update(float fTimeDeltaSeconds);
        static void shutdown();
        static void errorCheck(FMOD_RESULT result); // check if there is an error in the code.

        // load both masterbank and masterbank.strings before any other (see documentation on fmod studio)
        void loadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags = FMOD_STUDIO_LOAD_BANK_NORMAL);
        void loadEvent(const std::string& strEventName); // better to load with guid instead
        void loadSound(const std::string& strSoundName,
            bool bIs3d = true,
            bool bIsLooping = false,
            bool bIsStreaming = false);
        void unLoadSound(const std::string& strSoundName);
        void unLoadEvent(const std::string& strEventName);
        void unLoadBank(const std::string& strBankName);

        // playback
        int playSound(const std::string& strSoundName, const glm::vec3& vPos = glm::vec3{ 0,0,0 }, float fVolumedB = 0.0f);
        void playEvent(const std::string& strEventName);

        // stop playback
        void stopChannel(int nChannelId);
        void stopEvent(const std::string& strEventName, bool bImmediate = false);
        void stopAllChannels();

        // check playback
        bool isPlaying(int nChannelId) const;
        bool isEventPlaying(const std::string& strEventName) const;

        //Parameters
        void set3dListenerAndOrientation(const glm::vec3& vPosition, const glm::vec3& vLook, const glm::vec3& vUp);
        void setChannel3dPosition(int nChannelId, const glm::vec3& vPosition);
        void setChannelVolume(int nChannelId, float fVolumedB = 0.f);
        void getEventParameter(const std::string& strEventName, const std::string& strParameterName, float* outParValue);
        void setEventParameter(const std::string& strEventName, const std::string& strParameterName, float fValue);

        //volume and misc
        static float dBToVolume(float dB) {
            return powf(10.f, 0.05f * dB);
        }

        static float volumeTodB(float volume) {
            return 20.f * log10f(volume);
        }
        static FMOD_VECTOR vectorToFmod(const glm::vec3& vPosition) {
            FMOD_VECTOR fVec;
            fVec.x = vPosition.x;
            fVec.y = vPosition.y;
            fVec.z = vPosition.z;
            return fVec;
        }

        ///TODO will need to fix the src to make these function properly

        struct SoundDefinition
        {
            std::string mSoundName;
            float fDefaultVolumedB;
            float fMinDistance;
            float fMaxDistance;
            bool bIs3d;
            bool bIsLooping;
            bool bIsStreaming;
        };

        //int playSound(int nSoundID, const glm::vec3& vPos = glm::vec3{0,0,0}, float fVolumedB = 0.0f);
        //void loadSound(int nSoundID);
        //void unLoadSound(int nSoundID);

        //int registerSound(const SoundDefinition& tSoundDef, bool bLoad = true);
        //void unregisterSound(int nSoundId);
    };
}

