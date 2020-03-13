#include "AudioLayer.h"
#include "AudioEngine.h"
#include "florp/app/Application.h"
#include "florp/app/Window.h"
#include "florp/app/Timing.h"

void AudioLayer::Initialize()
{
	// TODO: Init the sound engine and load the master bank
	AudioEngine& audioEngine = AudioEngine::GetInstance();

	audioEngine.Init(); // getting the singleton.
	audioEngine.LoadBank("Master");

	audioEngine.LoadEvent(audioEvent);
	// audioEngine.PlayEvent(audioEvent);
	AudioEngine::GetInstance().SetEventPosition(audioEvent, startPos);
}

void AudioLayer::Shutdown()
{
	//TODO: Shutdown AudioEngine

	AudioEngine::GetInstance().Shutdown();
}

// linerar interpolation
float AudioLayer::Lerp(float a, float b, float u) { return glm::mix(a, b, u); }

// ease in interpolation. The higher the exponent, the greater the ease in and steeper the curve.
// see the reference for comparisons
float AudioLayer::EaseInLerp(float a, float b, float u)
{
	// y = x^n
	return glm::mix(a, b, powf(u, 5));
}

// ease out interpolation
float AudioLayer::EaseOutLerp(float a, float b, float u)
{
	// y = 1 - (1 - x)^n
	return glm::mix(a, b, 1.0F - powf(1.0F - u, 8));
}

// ease in and out
float AudioLayer::EaseInOutLerp(float a, float b, float u)
{
	float t = (u < 0.5F) ? 
		128.0F * (pow(u,8)) :
		0.5F + (1.0F - powf((2.0F * (1.0F - u)), 8)) / 2.0F;

	return glm::mix(a, b, t);
}

void AudioLayer::Update()
{
	using namespace florp::app;

	Window::Sptr window = Application::Get()->GetWindow();
	float deltaTime = Timing::DeltaTime;
	AudioEngine& audioEngine = AudioEngine::GetInstance();

	// key presses
	{
		// plays the event
		if (window->IsKeyDown(Key::P))
		{
			audioEngine.PlayEvent(audioEvent);
			playing = true;

			elapsedTime = 0.0F;
			u = 0.0F; // time value
			currPos = glm::vec3();
		}
	}

	// checks to see if the event is still playing
	// this function doesn't work for some reason.
	// playing = audioEngine.isEventPlaying(audioEvent);
	// std::cout << playing << std::endl;

	// if the audio is playing
	if (playing)
	{
		elapsedTime += deltaTime;
		// startup is 3 seconds (whole effect is 3.5 seconds)
		// ending crash starts 11 seconds in

		// repositing; stops at 11 seconds.
		// if (elapsedTime < 11000.0F)
		if (u < 1.0F) // crashed into wall
		{
			// currPos.x = EaseInLerp(startPos.x, endPos.x, u);
			currPos.x = EaseInLerp(startPos.x, endPos.x, u);
			u += U_INC * deltaTime;
			AudioEngine::GetInstance().SetEventPosition(audioEvent, currPos);
		}
		else
		{
			playing = false;
			// std::cout << "STOP" << std::endl;
		}
	}

	audioEngine.Update();
}
