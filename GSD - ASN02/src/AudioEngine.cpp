#include "AudioEngine.h"

//////// FMOD Implementation ////////

Implementation* implementation = nullptr;

Implementation::Implementation()
{
	mpStudioSystem = NULL;
	AudioEngine::ErrorCheck(FMOD::Studio::System::create(&mpStudioSystem));
	AudioEngine::ErrorCheck(mpStudioSystem->initialize(32, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_3D_RIGHTHANDED, NULL));

	mpSystem = NULL;
	AudioEngine::ErrorCheck(mpStudioSystem->getCoreSystem(&mpSystem));

	mnNextChannelId = 0;
} 

Implementation::~Implementation()
{
	AudioEngine::ErrorCheck(mpStudioSystem->unloadAll());
	AudioEngine::ErrorCheck(mpStudioSystem->release());
}

void Implementation::Update()
{
	std::vector<ChannelMap::iterator> pStoppedChannels;
	for (auto it = mChannels.begin(), itEnd = mChannels.end(); it != itEnd; ++it)
	{
		bool bIsPlaying = false;
		it->second->isPlaying(&bIsPlaying);
		if (!bIsPlaying)
		{
			pStoppedChannels.push_back(it);
		}
	}

	for (auto& it : pStoppedChannels)
	{
		mChannels.erase(it);
	}

	AudioEngine::ErrorCheck(mpStudioSystem->update());
}

//////// Logistics ////////

void AudioEngine::Init()
{
	implementation = new Implementation;
	LoadGUIDs();
}

void AudioEngine::LoadGUIDs()
{
	// Open the file
	std::ifstream guidFile;
	guidFile.open("GUIDs.txt");

	// Loop for each line
	if (guidFile.is_open())
	{
		std::string currentLine;
		std::string guid;
		std::string key;
		
		while (!guidFile.eof())
		{	
			// Get Line
			getline(guidFile, currentLine); // Format: "{bb98735b-7f42-4b9a-a178-7fe3140e7ea5} event:/Glide"
		
			// Parse GUID
			guid = currentLine.substr(0, currentLine.find(" "));

			// Parse Key
			key = currentLine.substr(currentLine.find(" ") + 1, currentLine.length() - guid.length() - 1);
			
			// Add to bank
			implementation->mGUIDs[key] = guid;
		}
		guidFile.close();
	}
	else
	{
		std::cout << "Audio Engine: GUID.txt not found" << std::endl;
	}
}

void AudioEngine::Shutdown()
{
	delete implementation;
}

void AudioEngine::Update()
{
	implementation->Update();
}

int AudioEngine::ErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		std::cout << "FMOD ERROR: " << FMOD_ErrorString(result) << std::endl;
		return 1;
	}

	// All good
	return 0;
}


//////// Banks ////////

void AudioEngine::LoadBank(const std::string& strBankName)
{
	auto tFoundIt = implementation->mBanks.find(strBankName);
	if (tFoundIt != implementation->mBanks.end())
		return;

	FMOD::Studio::Bank* pBank;
	AudioEngine::ErrorCheck(implementation->mpStudioSystem->loadBankFile(("Desktop/" + strBankName + ".bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &pBank));

	if (pBank)
	{
		implementation->mBanks[strBankName] = pBank;
	}
}

void AudioEngine::UnloadAllBanks()
{
	AudioEngine::ErrorCheck(implementation->mpStudioSystem->unloadAll());
}


//////// Events ////////

void AudioEngine::LoadEvent(const std::string& strEventName)
{
	// Return if the event is already loaded
	auto tFoundEvent = implementation->mEvents.find(strEventName);
	if (tFoundEvent != implementation->mEvents.end())
		return;

	// Return if the GUID does not exist
	auto tFoundGUID = implementation->mGUIDs.find("event:/" + strEventName);
	if (tFoundGUID == implementation->mGUIDs.end())
		return;
	
	// Get GUID
	std::string newEventGUID = implementation->mGUIDs["event:/" + strEventName];

	// Load event using the GUID
	FMOD::Studio::EventDescription* pEventDescription = NULL;
	AudioEngine::ErrorCheck(implementation->mpStudioSystem->getEvent(newEventGUID.c_str(), &pEventDescription));
	if (pEventDescription)
	{
		// Create event instance
		FMOD::Studio::EventInstance* pEventInstance = NULL;
		AudioEngine::ErrorCheck(pEventDescription->createInstance(&pEventInstance));
		if (pEventInstance)
		{
			implementation->mEvents[strEventName] = pEventInstance;
		}
	}
}

void AudioEngine::PlayEvent(const std::string& strEventName)
{
	// If event not found, load the event
	auto tFoundIt = implementation->mEvents.find(strEventName);
	if (tFoundIt == implementation->mEvents.end())
		return;

	tFoundIt->second->start();
}

void AudioEngine::StopEvent(const std::string& strEventName, bool bFadeOut)
{
	auto tFoundIt = implementation->mEvents.find(strEventName);
	if (tFoundIt == implementation->mEvents.end())
		return;

	FMOD_STUDIO_STOP_MODE eMode;
	eMode = bFadeOut ? FMOD_STUDIO_STOP_ALLOWFADEOUT : FMOD_STUDIO_STOP_IMMEDIATE;
	AudioEngine::ErrorCheck(tFoundIt->second->stop(eMode));
}

void AudioEngine::SetEventPosition(const std::string& strEventName, const glm::vec3 vPosition)
{
	// Get instance from map
	auto tFoundIt = implementation->mEvents.find(strEventName);
	if (tFoundIt == implementation->mEvents.end())
		return;

	// Temp Object
	FMOD_3D_ATTRIBUTES newAttributes;

	// Get attribute from event
	AudioEngine::ErrorCheck(tFoundIt->second->get3DAttributes(&newAttributes));

	// Set the new position
	newAttributes.position = VectorToFmod(vPosition);

	// Set new attribute on event
	AudioEngine::ErrorCheck(tFoundIt->second->set3DAttributes(&newAttributes));
}

bool AudioEngine::isEventPlaying(const std::string& strEventName) const
{
	auto tFoundIt = implementation->mEvents.find(strEventName);
	if (tFoundIt == implementation->mEvents.end())
		return false;

	FMOD_STUDIO_PLAYBACK_STATE* state = NULL;
	if (tFoundIt->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING) //help
	{
		return true;
	}

	return false;
}

//////// FMOD Parameters ////////

void AudioEngine::GetEventParameter(const std::string& strEventName, const std::string& strParameterName, float* parameter)
{
	auto tFoundIt = implementation->mEvents.find(strEventName);
	if (tFoundIt == implementation->mEvents.end())
		return;

	AudioEngine::ErrorCheck(tFoundIt->second->getParameterByName(strParameterName.c_str(), parameter));
}

void AudioEngine::SetEventParameter(const std::string& strEventName, const std::string& strParameterName, float fValue)
{
	auto tFoundIt = implementation->mEvents.find(strEventName);
	if (tFoundIt == implementation->mEvents.end())
		return;

	AudioEngine::ErrorCheck(tFoundIt->second->setParameterByName(strParameterName.c_str(), fValue));
}

void AudioEngine::SetGlobalParameter(const std::string& strParameterName, float fValue)
{
	AudioEngine::ErrorCheck(implementation->mpStudioSystem->setParameterByName(strParameterName.c_str(), fValue));

}


//////// Listeners ////////

void AudioEngine::SetListenerPosition(const glm::vec3& vPosition)
{
	// Temp Object
	FMOD_3D_ATTRIBUTES tempAttributes;

	// Get attribute from event
	AudioEngine::ErrorCheck(implementation->mpStudioSystem->getListenerAttributes(0, &tempAttributes));

	// Set the new position
	tempAttributes.position = VectorToFmod(vPosition);
	AudioEngine::ErrorCheck(implementation->mpStudioSystem->setListenerAttributes(0, &tempAttributes));

}

void AudioEngine::SetListenerOrientation(const glm::vec3& vUp, const glm::vec3& vForward)
{
	// Temp Object
	FMOD_3D_ATTRIBUTES tempAttributes;

	// Get attribute from event
	AudioEngine::ErrorCheck(implementation->mpStudioSystem->getListenerAttributes(0, &tempAttributes));

	// Set the new position
	tempAttributes.forward = VectorToFmod(vForward);
	tempAttributes.up = VectorToFmod(vUp);
	AudioEngine::ErrorCheck(implementation->mpStudioSystem->setListenerAttributes(0, &tempAttributes));

}


//////// Helpers /////////

FMOD_VECTOR AudioEngine::VectorToFmod(const glm::vec3& vPosition)
{
	FMOD_VECTOR fVec;
	fVec.x = vPosition.x;
	fVec.y = vPosition.y;
	fVec.z = vPosition.z;
	return fVec;
}

float AudioEngine::dbToVolume(float db)
{
	return powf(10.0f, 0.05f * db);
}

float AudioEngine::VolumeTodb(float volume)
{
	return 20.0f * log10f(volume);
}