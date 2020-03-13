#include "AudioLayer.h"
#include "AudioEngine.h"

void AudioLayer::Initialize()
{
	// TODO: Init the sound engine and load the master bank
	AudioEngine& audioEngine = AudioEngine::GetInstance();

	audioEngine.Init(); // getting the singleton.
	audioEngine.LoadBank("Master");

	audioEngine.LoadEvent("Car Crash");
	audioEngine.PlayEvent("Car Crash");
}

void AudioLayer::Shutdown()
{
	//TODO: Shutdown AudioEngine

	AudioEngine::GetInstance().Shutdown();
}

void AudioLayer::Update()
{
	//TODO: Update AudioEngine
	AudioEngine::GetInstance().Update();
}
