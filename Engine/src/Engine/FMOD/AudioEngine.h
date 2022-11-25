#pragma once
/*
 * Code is used from https://www.codyclaborn.me/tutorials/making-a-basic-fmod-audio-engine-in-c/
 * which is referring to this \/
 * Code is used from Guy Somberg's "Game Audio Programming: Principles and practices"
 * ISBN-13: 978-1-4987-4673-1
 */

#include "fmod.hpp"
#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_common.h"
#include "fmod_dsp.h"
#include "fmod_dsp_effects.h"
#include "fmod_errors.h"
#include "fmod_output.h"
#include "fmod_studio.hpp"
#include "fmod_studio.h"
#include "fmod_studio_common.h"
#include "fsbank.h"
#include "fsbank_errors.h"
#include <glm/glm.hpp>
#include <string>
#include <map>
#include <math.h> // to changeoctave & changesemitone
#include <iostream>

namespace Engine {

    //#define FMOD_VERSION 0x00020210

    struct ReverbProperties
    {
        float DecayTime;
        float EarlyDelay;
        float LateDelay;
        float HFReference;
        float HFDecayRatio;
        float Diffusion;
        float Density;
        float LowShelfFrequency;
        float LowShelfGain;
        float HighCut;
        float EarlyLateMix;
        float WetLevel;
        ReverbProperties(float _DecayTime, float _EarlyDelay, float _LateDelay, float _HFReference, float _HFDecayRatio,
            float _Diffusion, float _Density, float _LowShelfFrequency, float _LowShelfGain, float _HighCut, float _EarlyLateMix,
            float _WetLevel)
        {
            DecayTime = _DecayTime;
            EarlyDelay = _EarlyDelay;
            LateDelay = _LateDelay;
            HFReference = _HFReference;
            HFDecayRatio = _HFDecayRatio;
            Diffusion = _Diffusion;
            Density = _Density;
            LowShelfFrequency = _LowShelfFrequency;
            LowShelfGain = _LowShelfGain;
            HighCut = _HighCut;
            EarlyLateMix = _EarlyLateMix;
            WetLevel = _WetLevel;
        }
    };

    // https://www.fmod.com/docs/2.02/api/core-api-system.html#fmod_preset_generic

    static const ReverbProperties ENVIRONMENT_UNDERWATER =       {  1500,    7,  11, 5000,  10, 100, 100, 250, 0,   500,  92,   7.0f };
    static const ReverbProperties ENVIRONMENT_GENERIC =          {  1500,    7,  11, 5000,  83, 100, 100, 250, 0, 14500,  96,  -8.0f };
    static const ReverbProperties ENVIRONMENT_PADDEDCELL =       {   170,    1,   2, 5000,  10, 100, 100, 250, 0,   160,  84,  -7.8f };
    static const ReverbProperties ENVIRONMENT_ROOM =             {   400,    2,   3, 5000,  83, 100, 100, 250, 0,  6050,  88,  -9.4f };
    static const ReverbProperties ENVIRONMENT_BATHROOM =         {  1500,    7,  11, 5000,  54, 100,  60, 250, 0,  2900,  83,   0.5f };
    static const ReverbProperties ENVIRONMENT_LIVINGROOM =       {   500,    3,   4, 5000,  10, 100, 100, 250, 0,   160,  58, -19.0f };
    static const ReverbProperties ENVIRONMENT_STONEROOM =        {  2300,   12,  17, 5000,  64, 100, 100, 250, 0,  7800,  71,  -8.5f };
    static const ReverbProperties ENVIRONMENT_AUDITORIUM =       {  4300,   20,  30, 5000,  59, 100, 100, 250, 0,  5850,  64, -11.7f };
    static const ReverbProperties ENVIRONMENT_CONCERTHALL =      {  3900,   20,  29, 5000,  70, 100, 100, 250, 0,  5650,  80,  -9.8f };
    static const ReverbProperties ENVIRONMENT_CAVE =             {  2900,   15,  22, 5000, 100, 100, 100, 250, 0, 20000,  59, -11.3f };
    static const ReverbProperties ENVIRONMENT_ARENA =            {  7200,   20,  30, 5000,  33, 100, 100, 250, 0,  4500,  80,  -9.6f };
    static const ReverbProperties ENVIRONMENT_HANGER =           { 10000,   20,  30, 5000,  23, 100, 100, 250, 0,  3400,  72,  -7.4f };
    static const ReverbProperties ENVIRONMENT_CARPETTEDHALLWAY = {   300,    2,  30, 5000,  10, 100, 100, 250, 0,   500,  56, -24.0f };
    static const ReverbProperties ENVIRONMENT_HALLWAY =          {  1500,    7,  11, 5000,  59, 100, 100, 250, 0,  7800,  87,  -5.5f };
    static const ReverbProperties ENVIRONMENT_STONECORRIDOR =    {   270,   13,  20, 5000,  79, 100, 100, 250, 0,  9000,  86,  -6.0f };
    static const ReverbProperties ENVIRONMENT_ALLEY =            {  1500,    7,  11, 5000,  86, 100, 100, 250, 0,  8300,  80,  -9.8f };
    static const ReverbProperties ENVIRONMENT_FOREST =           {  1500,  162,  88, 5000,  54,  79, 100, 250, 0,   760,  94, -12.3f };            //For tech demo!!!
    static const ReverbProperties ENVIRONMENT_CITY =             {  1500,    7,  11, 5000,  67,  50, 100, 250, 0,  4050,  66, -26.0f };
    static const ReverbProperties ENVIRONMENT_MOUNTAINS =        {  1500,  300, 100, 5000,  21,  27, 100, 250, 0,  1220,  82, -24.0f };
    static const ReverbProperties ENVIRONMENT_QUARRY =           {  1500,   61,  25, 5000,  83, 100, 100, 250, 0,  3400, 100,  -5.0f };
    static const ReverbProperties ENVIRONMENT_PLAIN =            {  1500,  179, 100, 5000,  50,  21, 100, 250, 0,  1670,  65, -28.0f };
    static const ReverbProperties ENVIRONMENT_PARKINGLOT =       {  1700,    8,  12, 5000, 100, 100, 100, 250, 0, 20000,  56, -19.5f };
    static const ReverbProperties ENVIRONMENT_SEWERPIPE =        {  2800,   14,  21, 5000,  14,  80,  60, 250, 0,  3400,  66,   1.2f };
    static const ReverbProperties ENVIRONMENT_REVERB_OFF =       {  1000,    7,  11, 5000, 100, 100, 100, 250, 0,    20,  96, -80.0f };

    struct Implementation {
        Implementation();
        ~Implementation();

        void update(float fTimeDeltaSeconds);

        FMOD::Studio::System* mpStudioSystem;
        FMOD::System* mpSystem;
        FMOD::Reverb3D* mpReverb;

        int mnNextChannelId{};                                                  // Id of the next channel a sound will play in

        //-------------
        // FMOD CORE //
        //-------------
        typedef std::map<std::string, FMOD::Sound*> SoundMap;                   // Map os all sounds tracks in the project and their file
        typedef std::map<int, FMOD::Channel*> ChannelMap;                       // Map of all channels ind its ID
        typedef std::map<std::string, FMOD::Channel*> SoundChannelMap;          // Map of all sounds to channels they play in
        
        //---------------
        /* FMOD Studio */
        //---------------
        typedef std::map<std::string, FMOD::Studio::EventInstance*> EventMap;   // Map of all FMOD Studio events
        typedef std::map<std::string, FMOD::Studio::Bank*> BankMap;             // Map of all fmod studio banks

        SoundMap mSounds;                                                       // Sound cache
        ChannelMap mChannels;                                                   // ChannelID cache
        SoundChannelMap mSoundChannels;                                         // reference of sounds and channels

        BankMap mBanks;                                                         // Audio bank cache
        EventMap mEvents;                                                       // Events cache
    };

    struct Channel
    {
        //Channel(Implementation& tImplementation, int nSoundId, const AudioEngine::SoundDefinition& tSoundDefinition,
        //    const glm::vec3& vPosition, float fVolumedB);
        enum class State
        {
            INITIALIZE, TOPLAY, LOADING, PLAYING, STOPPING, STOPPED, VIRTUALIZING, VIRUTAL, DEVIRTUALIZE,
        };
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
        //void update(float fTimeDeltaSeconds);
        void updateChannelParameters();
        bool shouldBeVirtual(bool bAllowOneShotVirtuals) const;
        bool isPlaying() const;
        float getVolumedB() const;
    };

    class AudioEngine
    {
    public:
        AudioEngine();
        ~AudioEngine();

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
        int playSound(const std::string& strSoundName, const glm::vec3& vPos = glm::vec3{ 0,0,0 }, float fVolumedB = 0.0f, float fFadeInTime = 0.0f);
        void playEvent(const std::string& strEventName);

        // stop playback
        void stopChannel(int nChannelId, float fFadeOutTime = 0.0f);
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
        void moveChannel3dPosition(int nChannelId, const glm::vec3& vVelocity);

        // Fades
        void fadeOutChannel(int nChannelId, float fadeOutTime = 0.0f, float fadeOutVolume = 0.0f);
        void fadeInChannel(int nChannelId, float fadeInTime = 0.0f, float fadeInVolume = 1.0f);

        // Pitch and Frequency
        void setPitch(int nChannelId, float fPitch = 1.0f);
        float getPitch(int nChannelId);
        void setFrequency(int nChannelId, float fFrequency);
        float getFrequency(int nChannelId);

        void setLowPassGain(int nChannelId, float gain = 1.0f);
        float getLowPassGain(int nChannelId);

        // Reverb
        void activateReverb(bool active);
        bool getReverbState() const;
        FMOD_REVERB_PROPERTIES setReverbProperties(const ReverbProperties& reverbProp) const;
        void setEnvironmentReverb(const FMOD_REVERB_PROPERTIES reverbProperties, const glm::vec3 vPosition, float fMinDistance, float fMaxDistance);

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

        FMOD_VECTOR velocityToFmod(float _x, float _y, float _z)
        {
            FMOD_VECTOR fVel;
            fVel.x = _x;
            fVel.y = _y;
            fVel.z = _z;

            return fVel;
        }

        // TODO: Need to fix code to function properly
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