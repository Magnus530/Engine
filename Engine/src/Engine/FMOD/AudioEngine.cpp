#include "epch.h"
#include "AudioEngine.h"

namespace Engine 
{
	//*********************** IMPLEMENTATION CODE*********************//
	//--------------------------------------------------------------
	// Initialize the FMOD system
	//--------------------------------------------------------------
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
		mpReverb = nullptr;
		mnNextChannelId = 0;
	}

	//--------------------------------------------------------------
	// Release FMOD resources
	//--------------------------------------------------------------
	Implementation::~Implementation()
	{
		// unloading and releasing all sound maps to save memory leak
		AudioEngine::errorCheck(mpStudioSystem->unloadAll());
		AudioEngine::errorCheck(mpStudioSystem->release());
	}

	//--------------------------------------------------------------
	// Update the FMOD system
	//--------------------------------------------------------------
	void Implementation::update(float fTimeDeltaSeconds)
	{
		// if(mChannels.size() != 0) {return;}				// debug test
		// iterate through channels.
		std::vector<ChannelMap::iterator> pStoppedChannels;

		for (auto it = mChannels.begin(), itEnd = mChannels.end(); it != itEnd; it++)
		{
			bool bIsPlaying = false;
			it->second->isPlaying(&bIsPlaying);				// if we call and errorcheck here, it will return an error
			if (!bIsPlaying)								// "take note" is a channel is stopped
				pStoppedChannels.push_back(it);
		}
		for (auto& it : pStoppedChannels)
			mChannels.erase(it);						// erase the stopped channel

		std::vector<SoundChannelMap::iterator> pStoppedSoundChannels;
		for (auto it = mSoundChannels.begin(), itEnd = mSoundChannels.end(); it != itEnd; it++)
		{
			bool bIsPlaying = false;
			it->second->isPlaying(&bIsPlaying);				// if we call and errorcheck here, it will return an error
			if (!bIsPlaying)								// "take note" is a channel is stopped
				pStoppedSoundChannels.push_back(it);
		}
		for (auto& it : pStoppedSoundChannels)
			mSoundChannels.erase(it);					// erase the stopped channel

		AudioEngine::errorCheck(mpStudioSystem->update());
	}

	// ************************* Static / Global initialisation of the audio engine *********************** //
	// static methodes because we want to init one instance only (as a singleton)
	// we don't provide to this class an instance of the implementation or a pointer to it
	// neither do we ovverride the copy constructor and the assingment operator ( to communicate with the impl class)
	
	Implementation* sgpImplementation = nullptr;



	AudioEngine::AudioEngine()
	{
		init();

		std::string oneShot1 = "assets/audio/sfx_sound.wav";
		std::string mx1 = "assets/audio/Cartoon_song.wav";

		loadSound(mx1, false, true, true);
		//loadSound(oneShot1 = "assets/audio/sfx_sound.wav", false, true, true);
		playSound(mx1, glm::vec3(), -6.f, 1.f);
	}

	AudioEngine::~AudioEngine()
	{
		shutdown();
	}

	//----------------------
	// Initialize the engine
	//----------------------
	void AudioEngine::init()
	{
		sgpImplementation = new Implementation;
	}

	////add sounds quickly (making it public to get it in all funtions)
	//std::string musicPath1 = "assets/audio/Cartoon_song.wav";
	//std::string	oneShot1 = "assets/audio/sfx_sound.wav";
	//if (Input::IsKeyPressed(E_KEY_Q))
	//{
	//	playSound(oneShot1, glm::vec3(), -6.f);
	//	setChannelVolume(musicPath1, 12.f);
	//}
	//------------------------------
	// Update the engine properties
	//------------------------------
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

	//-----------------
	// Load bank files
	//-----------------
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
	
	//-------------------------
	// Load FMOD studio events
	//-------------------------
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

	//--------------------------------------------------------
	// Load a sound track and store in the queue for playback
	//--------------------------------------------------------
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
		//eMode |= FMOD_INIT_CHANNEL_LOWPASS;												// enable low pass filtering
		FMOD::Sound* pSound = nullptr;
		AudioEngine::errorCheck(sgpImplementation->mpSystem->createSound(strSoundName.c_str(), eMode, nullptr, &pSound));
		// If sound loaded correctly
		// add it to the queue of all sounds (cache)
		if (pSound)
			sgpImplementation->mSounds[strSoundName] = pSound;
	}
	
	//----------------------------
	// Unload sounds in the cache
	//----------------------------
	void AudioEngine::unLoadSound(const std::string& strSoundName)
	{
		// check cache for sound
		auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);			// Find sound  from the list of loaded files (cache)
		if (tFoundIt != sgpImplementation->mSounds.end())
			return;

		// Clear sound after use
		AudioEngine::errorCheck(tFoundIt->second->release());					// check for errors and unload sound
		sgpImplementation->mSounds.erase(tFoundIt);								// erase from the cache
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
	
	// PLAYBACK
	// ----------------------------------------------------------------
	// Play sounds that have been loaded correctly
	// create channels for the sound to play
	// sound is initially paused to avoid the sound popping in abruptly
	// returns the channel ID of playing sound
	// ----------------------------------------------------------------
	int AudioEngine::playSound(const std::string& strSoundName, const glm::vec3& vPos, float fVolumedB,float fFadeInTime)
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

			sgpImplementation->mChannels[nChannelId] = pChannel;
			this->fadeInChannel(nChannelId, fFadeInTime, dBToVolume(fVolumedB));
			sgpImplementation->mSoundChannels[strSoundName] = pChannel;
			AudioEngine::errorCheck(pChannel->setPaused(false));
		}
		return nChannelId;
	}

	//------------
	// Play Events
	//------------
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

	//----------------------------
	// Stops the specified channel
	//----------------------------
	void AudioEngine::stopChannel(int nChannelId, float fFadeOutTime)
	{
		bool playing;
		auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
		if (tFoundIt == sgpImplementation->mChannels.end())
			return;
		tFoundIt->second->isPlaying(&playing);
		if (playing)
		{
			if (fFadeOutTime <= 0.0f)
				AudioEngine::errorCheck(tFoundIt->second->stop());
			else
				this->fadeOutChannel(tFoundIt->first, fFadeOutTime, 0);
		}
	}
	
	//------------
	// Stop Events
	//------------
	void AudioEngine::stopEvent(const std::string& strEventName, bool bImmediate)
	{
		auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
		if (tFoundIt == sgpImplementation->mEvents.end())
			return;
		FMOD_STUDIO_STOP_MODE eMode;
		eMode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
		AudioEngine::errorCheck(tFoundIt->second->stop(eMode));
	}

	//-------------------------------------
	// Stops all currently playing channels
	//-------------------------------------
	void AudioEngine::stopAllChannels()
	{
		for (auto [name, channel] : sgpImplementation->mChannels)
			AudioEngine::errorCheck(channel->stop());
	}
	
	//------------------------------------------------
	// Return true if the channel is currently playing
	//------------------------------------------------
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

	//------------------------------
	// Check if the event is playing
	//------------------------------
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

	//-------------------------------------------------------------------------------
	// Set the position of the camera and hear sound based on where it is positioned.
	//-------------------------------------------------------------------------------
	void AudioEngine::set3dListenerAndOrientation(const glm::vec3& vPosition, const glm::vec3& vLook, const glm::vec3& vUp)
	{
		FMOD_VECTOR position = vectorToFmod(vPosition);
		FMOD_VECTOR forward = vectorToFmod(vLook);
		FMOD_VECTOR up = vectorToFmod(vUp);
		FMOD_VECTOR velocity = velocityToFmod(0,0,0);
		AudioEngine::errorCheck(sgpImplementation->mpSystem->set3DListenerAttributes(0, &position, &velocity, &forward, &up));
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
	
	//------------------------------------
	// Set the volume of the sound channel
	//------------------------------------
	void AudioEngine::setChannelVolume(int nChannelId, float fVolumedB)
	{
		auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
		if (tFoundIt == sgpImplementation->mChannels.end())
			return;
		AudioEngine::errorCheck(tFoundIt->second->setVolume(dBToVolume(fVolumedB)));
	}
	
	//------------
	// Play Events
	//------------
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

	//---------------------------------
	// Set the parameters of the events
	//---------------------------------
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

	//----------------------------------------
	// Move audio source position in 3D space
	//----------------------------------------
	void AudioEngine::moveChannel3dPosition(int nChannelId, const glm::vec3& vVelocity)
	{
		auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
		if (tFoundIt == sgpImplementation->mChannels.end())
			return;
		FMOD_VECTOR currentPosition;
		tFoundIt->second->get3DAttributes(&currentPosition, nullptr);
		currentPosition.x += vectorToFmod(vVelocity).x;
		currentPosition.y += vectorToFmod(vVelocity).y;
		currentPosition.z += vectorToFmod(vVelocity).z;
		AudioEngine::errorCheck(tFoundIt->second->set3DAttributes(&currentPosition, nullptr));
	}

	//--------------------------------------------------------------------
	// Fade out a given channel to a given volume
	// Stops the channel when fadeouttime is zero
	// FadeOutTime is used to set the time over which the sound will fade
	//--------------------------------------------------------------------
	void AudioEngine::fadeOutChannel(int nChannelId, float fadeOutTime, float fadeOutVolume)
	{
		bool bPlaying;
		float fCurrentVolume;
		auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
		if (tFoundIt == sgpImplementation->mChannels.end())
			return;
		AudioEngine::errorCheck(tFoundIt->second->isPlaying(&bPlaying));
		AudioEngine::errorCheck(tFoundIt->second->getVolume(&fCurrentVolume));
		if (bPlaying)
		{
			unsigned long long dspClock;
			int rate;
			FMOD::System* sys;

			if (fadeOutTime <= 0.0f)
				AudioEngine::errorCheck(tFoundIt->second->stop());
			else
			{
				AudioEngine::errorCheck(tFoundIt->second->getSystemObject(&sys));
				AudioEngine::errorCheck(sys->getSoftwareFormat(&rate, 0, 0));
				AudioEngine::errorCheck(tFoundIt->second->getDSPClock(0, &dspClock));
				AudioEngine::errorCheck(tFoundIt->second->addFadePoint(dspClock, fCurrentVolume));
				AudioEngine::errorCheck(tFoundIt->second->addFadePoint(dspClock + (rate * fadeOutTime), fadeOutVolume));
				AudioEngine::errorCheck(tFoundIt->second->setDelay(0, dspClock + (rate * fadeOutTime), true));
			}
		}
		tFoundIt->second->isPlaying(&bPlaying);
		if (!bPlaying)
			sgpImplementation->mnNextChannelId--;
	}

	//------------------------------------------------------------------
	// Fade in a given channel to a given volume from zero volume
	// FadeInTime is used to set the time over which the sound will fade
	//-------------------------------------------------------------------
	void AudioEngine::fadeInChannel(int nChannelId, float fadeInTime, float fadeInVolume)
	{
		auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
		if (tFoundIt == sgpImplementation->mChannels.end())
			return;
		unsigned long long dspClock;
		int rate;
		FMOD::System* sys;
		AudioEngine::errorCheck(tFoundIt->second->getSystemObject(&sys));
		AudioEngine::errorCheck(sys->getSoftwareFormat(&rate, 0, 0));
		AudioEngine::errorCheck(tFoundIt->second->getDSPClock(0, &dspClock));
		AudioEngine::errorCheck(tFoundIt->second->addFadePoint(dspClock, 0));
		AudioEngine::errorCheck(tFoundIt->second->addFadePoint(dspClock + (rate * fadeInTime), fadeInVolume));
	}

	//----------------------------------------------------------------------
	// Set the pitch of a channel
	// Pitch value, 0.5 = half pitch, 2.0 = double pitch, etc default = 1.0.
	//----------------------------------------------------------------------
	void AudioEngine::setPitch(int nChannelId, float fPitch)
	{
		auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
		if (tFoundIt == sgpImplementation->mChannels.end())
			return;
		AudioEngine::errorCheck(tFoundIt->second->setPitch(fPitch));
	}

	//--------------------------------------
	// Return the pitch of the given channel
	//--------------------------------------
	float AudioEngine::getPitch(int nChannelId)
	{
		auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
		if (tFoundIt == sgpImplementation->mChannels.end())
			return 0;
		float fPitch = 0;
		AudioEngine::errorCheck(tFoundIt->second->getPitch(&fPitch));
		return fPitch;
	}

	//-------------------------------
	// Set the frequency of a channel
	//-------------------------------
	void AudioEngine::setFrequency(int nChannelId, float fFrequency)
	{
		auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
		if (tFoundIt == sgpImplementation->mChannels.end())
			return;
		AudioEngine::errorCheck(tFoundIt->second->setFrequency(fFrequency));
	}

	//-------------------------------------------------------
	// Return the frequency of the given channel
	// Frequency value in Hertz
	// If frequency is negative the sound will play backwards
	//-------------------------------------------------------
	float AudioEngine::getFrequency(int nChannelId)
	{
		auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
		if (tFoundIt == sgpImplementation->mChannels.end())
			return 0;
		float fFrequency = 0;
		AudioEngine::errorCheck(tFoundIt->second->getFrequency(&fFrequency));
		return fFrequency;
	}

	//-----------------------------------
	// Set the low pass gain of a channel
	//-----------------------------------
	void AudioEngine::setLowPassGain(int nChannelId, float gain)
	{
		auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
		if (tFoundIt == sgpImplementation->mChannels.end())
			return;
		AudioEngine::errorCheck(tFoundIt->second->setLowPassGain(gain));
	}

	//------------------------------------------------------------------------------------------------------
	// Return the low pass gain of the given channel
	// Linear gain level, from 0 (silent, full filtering) to 1.0 (full volume, no filtering), default = 1.0.
	//------------------------------------------------------------------------------------------------------
	float AudioEngine::getLowPassGain(int nChannelId)
	{
		auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
		if (tFoundIt == sgpImplementation->mChannels.end())
			return 0;
		float fGain = 0;
		AudioEngine::errorCheck(tFoundIt->second->getLowPassGain(&fGain));
		return fGain;
	}


	//----------------------------
	// Activate environment reverb
	//----------------------------
	void AudioEngine::activateReverb(bool active)
	{
		AudioEngine::errorCheck(sgpImplementation->mpReverb->setActive(&active));
	}

	//----------------------------------
	// Return if reverb is active or not
	//----------------------------------
	bool AudioEngine::getReverbState() const
	{
		bool active;
		AudioEngine::errorCheck(sgpImplementation->mpReverb->getActive(&active));
		return active;
	}

	//----------------------
	// Set reverb properties
	//----------------------
	FMOD_REVERB_PROPERTIES AudioEngine::setReverbProperties(const ReverbProperties& reverbProp) const
	{

		FMOD_REVERB_PROPERTIES myReverbProp = { reverbProp.DecayTime, reverbProp.EarlyDelay, reverbProp.LateDelay, reverbProp.HFReference,
												reverbProp.HFDecayRatio, reverbProp.Diffusion, reverbProp.Density, reverbProp.LowShelfFrequency,
												reverbProp.LowShelfGain, reverbProp.HighCut, reverbProp.EarlyLateMix, reverbProp.WetLevel };
		return myReverbProp;
	}

	//---------------------------------------------------------------------------------------
	// Set a 3D reverb zone having the given reverb properties
	// The zone will be a sphere centered at the gievn position
	// The reverb effect will be heard from min distance from the center to the max distance
	//---------------------------------------------------------------------------------------
	void AudioEngine::setEnvironmentReverb(const FMOD_REVERB_PROPERTIES reverbProperties, const glm::vec3 vPosition, float fMinDistance, float fMaxDistance)
	{
		AudioEngine::errorCheck(sgpImplementation->mpSystem->createReverb3D(&sgpImplementation->mpReverb));
		sgpImplementation->mpReverb->setProperties(&reverbProperties);
		FMOD_VECTOR pos = vectorToFmod(vPosition);
		float mindist = fMinDistance;
		float maxdist = fMaxDistance;
		AudioEngine::errorCheck(sgpImplementation->mpReverb->set3DAttributes(&pos, mindist, maxdist));
	}
/*
	Channel::Channel(Implementation& tImplementation, int nSoundId, const AudioEngine::SoundDefinition& tSoundDefinition, const glm::vec3& vPosition, float fVolumedB)
	{
		

	}

	///TODO: WIP of changing states. Will make things easier
	void Channel::update(float fTimeDeltaSeconds)
	{
		switch(meState)
		{
		case Channel::State::INITIALIZE:
			[[fallthrough]];
		case Channel::State::DEVIRTUALIZE:
		case Channel::State::TOPLAY:
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
		case Channel::State::LOADING:
			if(mImplementation.soundIsLoaded(mSoundId))
				meState = State::TOPLAY;
			break;
		case Channel::State::PLAYING:
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
		case Channel::State::STOPPING:
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
		case Channel::State::STOPPED: break;
		case Channel::State::VIRTUALIZING:
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
		case Channel::State::VIRUTAL:
			if(mbStopRequested)
				meState = State::STOPPING;
			else if(!shouldBeVirtual(false))
				meState = State::DEVIRTUALIZE;
			break;
	  }
	}
	*/
	void Channel::updateChannelParameters()
	{
	}
	bool Channel::shouldBeVirtual(bool bAllowOneShotVirtuals) const
	{
		return false;
	}
	bool Channel::isPlaying() const
	{
		return false;
	}
	float Channel::getVolumedB() const
	{
		return 0.0f;
	}

	/*
	void AudioEngine::loadSound(int nSoundID)
	{
		if (soundIsLoaded(nSoundID))
			return;
		auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
		if (tFoundIt != sgpImplementation->mSounds.end())
			return;
		FMOD_MODE eMode = FMOD_NONBLOCKING;
		eMode |= b3d ? (FMOD_3D | FMOD_3D_INVERSETAPEREDROLLOFF) : FMOD_2D;
		eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;
		FMOD::Sound* pSound = nullptr;
		AudioEngine::errorCheck(sgpImplementation->mpSystem->createSound(strSoundName.c_str(), eMode, nullptr, &pSound));
		if (pSound)
			sgpImplementation->mSounds[strSoundName] = pSound;
	}
	*/
}