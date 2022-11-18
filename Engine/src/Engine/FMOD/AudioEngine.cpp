#include "epch.h"
#include "AudioEngine.h"

namespace Engine 
{
	//*********************** IMPLEMENTATION CODE*********************//
	Implementation::Implementation()
	{
		// Create a System studio object and initialize.
		mpStudioSystem = nullptr;
		AudioEngine::errorCheck(FMOD::Studio::System::create(&mpStudioSystem));
		AudioEngine::errorCheck(mpStudioSystem->initialize(
			32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, nullptr));

		// Create a System object.
		mpSystem = nullptr;
		AudioEngine::errorCheck(mpStudioSystem->getCoreSystem(&mpSystem));
	}

	Implementation::~Implementation()
	{
		// unloading and releasing all sound maps to save memory leak
		AudioEngine::errorCheck(mpStudioSystem->unloadAll());
		AudioEngine::errorCheck(mpStudioSystem->release());
	}
	void Implementation::update(float fTimeDeltaSeconds)
	{
		// if(mChannels.size() != 0) {return;}    // debug test
		// iterate through channels.
		std::vector<ChannelMap::iterator> pStoppedChannels;

		for (auto it = mChannels.begin(), itEnd = mChannels.end(); it != itEnd; ++it)
		{
			bool bIsPlaying = false;
			it->second->isPlaying(&bIsPlaying);     // if we call and errorcheck here, it will return an error
			if (!bIsPlaying) // "take note" is a channel is stopped
				pStoppedChannels.push_back(it);
		}
		for (auto& it : pStoppedChannels)
			mChannels.erase(it); // erase the stopped channel
		AudioEngine::errorCheck(mpStudioSystem->update());
	}

	// ************************* Static / Global initialisation of the audio engine *********************** //
	// static methodes because we want to init one instance only (as a singleton)
	// we don't provide to this class an instance of the implementation or a pointer to it
	// neither do we ovverride the copy constructor and the assingment operator ( to communicate with the impl class)
	
	Implementation* sgpImplementation = nullptr;

	void AudioEngine::init()
	{
		sgpImplementation = new Implementation;
	}

	void AudioEngine::update(float fTimeDeltaSeconds)
	{
		sgpImplementation->update(fTimeDeltaSeconds);
	}

	void AudioEngine::shutdown()
	{
		delete sgpImplementation;
	}

	// interface code, and  functions definitions

	// check for errors
	void AudioEngine::errorCheck(FMOD_RESULT result)
	{
		if (result != FMOD_OK)
		{
			std::printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
			exit(-1);
		}
	}

	void AudioEngine::loadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
	{
		auto tFoundIt = sgpImplementation->mBanks.find(strBankName);
		if (tFoundIt != sgpImplementation->mBanks.end())
			return;
		FMOD::Studio::Bank* pBank;
		AudioEngine::errorCheck(sgpImplementation->mpStudioSystem->loadBankFile(strBankName.c_str(), flags, &pBank));
		if (pBank)
			sgpImplementation->mBanks[strBankName] = pBank; // assign bank to bankmap
	}
	
	void AudioEngine::loadEvent(const std::string& strEventName)
	{
		auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
		if (tFoundIt != sgpImplementation->mEvents.end())
			return;
		FMOD::Studio::EventDescription* pEventDescription = nullptr;
		AudioEngine::errorCheck(sgpImplementation->mpStudioSystem->getEvent(strEventName.c_str(), &pEventDescription));
		if (pEventDescription) {
			FMOD::Studio::EventInstance* pEventInstance = nullptr;
			AudioEngine::errorCheck(pEventDescription->createInstance(&pEventInstance));
			if (pEventInstance)
				sgpImplementation->mEvents[strEventName] = pEventInstance;
		}
	}
	
	void AudioEngine::loadSound(const std::string& strSoundName, bool bIs3d, bool bIsLooping, bool bIsStreaming)
	{
		FMOD_CREATESOUNDEXINFO exinfo;
		memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
		exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
		exinfo.numchannels = 2;
		exinfo.defaultfrequency = 48000;
		exinfo.length = exinfo.defaultfrequency * exinfo.numchannels * sizeof(signed short) * 5;
		exinfo.format = FMOD_SOUND_FORMAT_PCM24;

		// check cache, if we allready have loaded the sound previously
		auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
		if (tFoundIt != sgpImplementation->mSounds.end())
			return;
		// set up modes for each sound category
		FMOD_MODE eMode = FMOD_DEFAULT;
		eMode |= bIs3d ? (FMOD_3D | FMOD_3D_INVERSETAPEREDROLLOFF) : FMOD_2D;
		eMode |= bIsLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		eMode |= bIsStreaming ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;
		FMOD::Sound* pSound = nullptr;
		AudioEngine::errorCheck(sgpImplementation->mpSystem->createSound(strSoundName.c_str(), eMode, nullptr, &pSound));
		if (pSound) // ass sound to the cache
			sgpImplementation->mSounds[strSoundName] = pSound;
	}
	
	void AudioEngine::unLoadSound(const std::string& strSoundName)
	{
		// check cache for sound
		auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
		if (tFoundIt != sgpImplementation->mSounds.end())
			return;
		AudioEngine::errorCheck(tFoundIt->second->release()); // check for errors and unload sound
		sgpImplementation->mSounds.erase(tFoundIt); // erase from the cache
	}
	
	void AudioEngine::unLoadEvent(const std::string& strEventName)
	{
		//check cache for sound events
		auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
		if (tFoundIt != sgpImplementation->mEvents.end())
			return;
		AudioEngine::errorCheck(tFoundIt->second->release()); // check for errors and unload events
		sgpImplementation->mEvents.erase(tFoundIt); // erase from the cache
	}
	
	void AudioEngine::unLoadBank(const std::string& strBankName)
	{
		// check cache for sound banks
		auto tFoundIt = sgpImplementation->mBanks.find(strBankName);
		if (tFoundIt != sgpImplementation->mBanks.end())
			return;
		AudioEngine::errorCheck(tFoundIt->second->unload()); // check for errors and unload banks
		sgpImplementation->mBanks.erase(tFoundIt); // erase from the cache
	}
	
	// playback

	int AudioEngine::playSound(const std::string& strSoundName, const glm::vec3& vPos, float fVolumedB)
	{
		int nChannelId = sgpImplementation->mnNextChannelId++;  // report next unused / free channelID
		// assign it to current channel and increase the counter
		auto tSoundIt = sgpImplementation->mSounds.find(strSoundName);
		if (tSoundIt == sgpImplementation->mSounds.end())
		{
			loadSound(strSoundName);
			tSoundIt = sgpImplementation->mSounds.find(strSoundName);
			if (tSoundIt == sgpImplementation->mSounds.end())
			{
				std::cout << "Could not find sound: " << strSoundName << std::endl; // give info if sound not found. Could use qdebug instead.
				return nChannelId;
			}
		}

		FMOD::Channel* pChannel = nullptr;
		AudioEngine::errorCheck(sgpImplementation->mpSystem->playSound(tSoundIt->second, nullptr, true, &pChannel));
		if (pChannel)
		{
			FMOD_MODE currMode;
			AudioEngine::errorCheck(tSoundIt->second->getMode(&currMode));
			if (currMode & FMOD_3D) {
				FMOD_VECTOR position = vectorToFmod(vPos);
				AudioEngine::errorCheck(pChannel->set3DAttributes(&position, nullptr));
			}
			AudioEngine::errorCheck(pChannel->setVolume(dBToVolume(fVolumedB)));
			AudioEngine::errorCheck(pChannel->setPaused(false));
			sgpImplementation->mChannels[nChannelId] = pChannel;
		}
		return nChannelId;
	}
	
	void AudioEngine::playEvent(const std::string& strEventName)
	{
		auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
		if (tFoundIt == sgpImplementation->mEvents.end()) {
			loadEvent(strEventName);
			tFoundIt = sgpImplementation->mEvents.find(strEventName);
			if (tFoundIt == sgpImplementation->mEvents.end())
				return;
		}
		AudioEngine::errorCheck(tFoundIt->second->start());
	}
	
	// stop playback

	void AudioEngine::stopChannel(int nChannelId)
	{
		auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
		if (tFoundIt == sgpImplementation->mChannels.end())
			return;
		AudioEngine::errorCheck(tFoundIt->second->stop());
	}
	
	void AudioEngine::stopEvent(const std::string& strEventName, bool bImmediate)
	{
		auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
		if (tFoundIt == sgpImplementation->mEvents.end())
			return;
		FMOD_STUDIO_STOP_MODE eMode;
		eMode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
		AudioEngine::errorCheck(tFoundIt->second->stop(eMode));
	}
	
	void AudioEngine::stopAllChannels()
	{
		for (auto [name, channel] : sgpImplementation->mChannels)
			AudioEngine::errorCheck(channel->stop());
	}
	
	bool AudioEngine::isPlaying(int nChannelId) const
	{
		auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
		if (tFoundIt == sgpImplementation->mChannels.end())
		{
			std::cout << "Channel ID: " << nChannelId << ", not found. \n";
			return false;
		}

		bool bIsPlaying = false;
		AudioEngine::errorCheck(tFoundIt->second->isPlaying(&bIsPlaying));
		if (bIsPlaying) //it should always be playing if found.
		{
			std::cout << "Channel ID: " << nChannelId << ", playing.\n";
			return bIsPlaying;
		}
		return bIsPlaying;
	}

	bool AudioEngine::isEventPlaying(const std::string& strEventName) const
	{
		auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
		if (tFoundIt == sgpImplementation->mEvents.end())
			return false;
		FMOD_STUDIO_PLAYBACK_STATE state;
		AudioEngine::errorCheck(tFoundIt->second->getPlaybackState(&state));
		if (state == FMOD_STUDIO_PLAYBACK_PLAYING)
			return true;

		return false;
	}

	// put the position of the camera and hear sound based on where it is positioned.

	void AudioEngine::set3dListenerAndOrientation(const glm::vec3& vPosition, const glm::vec3& vLook, const glm::vec3& vUp)
	{
		FMOD_VECTOR position = vectorToFmod(vPosition);
		FMOD_VECTOR look = vectorToFmod(vLook);
		FMOD_VECTOR up = vectorToFmod(vUp);
		AudioEngine::errorCheck(sgpImplementation->mpSystem->set3DListenerAttributes(0, &position, nullptr, &look, &up));
	}
	
	// parameters 

	void AudioEngine::setChannel3dPosition(int nChannelId, const glm::vec3& vPosition)
	{
		auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
		if (tFoundIt == sgpImplementation->mChannels.end())
			return;
		FMOD_VECTOR position = vectorToFmod(vPosition);
		AudioEngine::errorCheck(tFoundIt->second->set3DAttributes(&position, nullptr));
	}
	
	void AudioEngine::setChannelVolume(int nChannelId, float fVolumedB)
	{
		auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
		if (tFoundIt == sgpImplementation->mChannels.end())
			return;
		AudioEngine::errorCheck(tFoundIt->second->setVolume(dBToVolume(fVolumedB)));
	}
	
	void AudioEngine::getEventParameter(const std::string& strEventName, const std::string& strParameterName, float* outParValue)
	{
		auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
		if (tFoundIt == sgpImplementation->mEvents.end())
			return;
		/*
		// get global parameter complete info, could also be retrieved from EventDescription
		FMOD_STUDIO_PARAMETER_DESCRIPTION *parameter = nullptr;
		sgpImplementation->mpStudioSystem->getParameterDescriptionByName(strParameterName.c_str(), parameter);
		// get global parameter value
		AudioEngine::errorCheck(sgpImplementation->mpStudioSystem->getParameterByName(strParameterName.c_str(), outParValue));
		*/

		// get local event parameter value
		AudioEngine::errorCheck(tFoundIt->second->getParameterByName(strParameterName.c_str(), outParValue));
	}
	
	void AudioEngine::setEventParameter(const std::string& strEventName, const std::string& strParameterName, float fValue)
	{
		auto tFoundIt = sgpImplementation->mEvents.find(strParameterName);
		if (tFoundIt == sgpImplementation->mEvents.end())
			return;


		//    // set local event parameter value
		//AudioEngine::errorCheck(sgpImplementation->mpStudioSystem->setParameterByName(strParameterName.c_str(), fValue));

		// set local event parameter value
		AudioEngine::errorCheck(tFoundIt->second->setParameterByName(strParameterName.c_str(), fValue));
	}

	///TODO: WIP of changing states. Will make things easier
	/*
	void Implementation::Channel::update(float fTimeDeltaSeconds)
	{
		switch(meState)
		{
		case Implementation::Channel::State::INITIALIZE:
			[[fallthrough]];
		case Implementation::Channel::State::DEVIRTUALIZE:
		case Implementation::Channel::State::TOPLAY:
		{
			if(mbStopRequested){
				meState = State::STOPPING;
				return;
			}
			if(shouldBeVirtual(true)){
				if(IsOneShot()){
					meState = State::STOPPING;
				} else {
					meState = State::VIRUTAL;
				}
				return;
			}
			if(!mImplementation.SoundIsLoaded(mSoundId)){
				mImplementation.loadSound(mSoundId);
				meState= State::LOADING;
				return;
			}
			mpChannel = nullptr;
			auto tSoundIt = mImplementation.mSounds.find(mSoundId);
			if(tSoundIt != mImplementation.mSounds.end())
				mImplementation.mpSystem->playSound(tSoundIt->Second->mpSound,nullptr, true, &mpChannel);
			if(mpChannel){
				if(meState == State::DEVIRTUALIZE)
					mVirtualizeFader.StartFade(SILENCE_dB, 0.0f, VIRTUALIZE_FADE_TIME);
				meState=State::PLAYING;
				FMOD_VECTOR position = AudioEngine::vectorToFmod(mvPosition);
				mpChannel->set3DAttributes(&position, nullptr);
				mpChannel->setVolume(dBToVolume(getVolumedB()));
				mpChannel->setPaused(false);
			}
			else
				meState = State::STOPPING;
		}
		break;
		case Implementation::Channel::State::LOADING:
			if(mImplementation.soundIsLoaded(mSoundId))
				meState = State::TOPLAY;
			break;
		case Implementation::Channel::State::PLAYING:
			mVirtualizeFader.Update(fTimeDeltaSeconds);
			updateChannelParameters();
			if(!isPlaying() || mbStopRequested)
			{
				meState = State::STOPPING;
				return;
			}
			if(shouldBeVirtual(false))
			{
				mVirtualizeFader.startFade(SILENCE_dB, VIRTUALIZE_FADE_TIME);
				meState = State::VIRTUALIZING;
			}
			break;
		case Implementation::Channel::State::STOPPING:
			mStopFader.Update(fTimeDeltaSeconds);
			updateChannelParameters();
			if(mStopFader.IsFinished()){
				mpChannel->stop();
			}
			if(!isPlaying())
			{
				meState = State::STOPPED;
				return;
			}
			break;
		case Implementation::Channel::State::STOPPED: break;
		case Implementation::Channel::State::VIRTUALIZING:
			mVirtualizeFader.update(fTimeDeltaSeconds);
			updateChannelParameters();
			if(!shouldBeVirtual(false))
			{
				mVirtualizeFader.startFade(0.0f, VIRTUALIZE_FADE_TIME);
				meState = State::PLAYING;
				break;
			}
			if(mVirtualizeFader.isFinished())
			{
				mpChannel->stop();
				meState = State::VIRUTAL;
			}
			break;
		case Implementation::Channel::State::VIRUTAL:
			if(mbStopRequested)
				meState = State::STOPPING;
			else if(!shouldBeVirtual(false))
				meState = State::DEVIRTUALIZE;
			break;
	  }
	}
	void AudioEngine::loadSound(int nSoundID)
	{
		if(soundIsLoaded(nSoundID))
			return;
	auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
	if(tFoundIt != sgpImplementation->mSounds.end())
		return;
	FMOD_MODE eMode= FMOD_NONBLOCKING;
	eMode |= b3d ? (FMOD_3D | FMOD_3D_INVERSETAPEREDROLLOFF) : FMOD_2D;
	eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;
	FMOD::Sound* pSound = nullptr;
	AudioEngine::errorCheck(sgpImplementation->mpSystem->createSound(strSoundName.c_str(), eMode, nullptr, &pSound));
	if(pSound)
		sgpImplementation->mSounds[strSoundName] = pSound;
	}
*/
}