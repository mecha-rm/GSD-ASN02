/*
 * Name: Team Bonus Fruit
	- Kennedy Adams	(100632983)
	- Jonah Griffin	(100702748)
	- Nathan Tuck	(100708651)
	- Stephane Gagnon (100694227)
	- Roderick "R.J." Montague (100701758)
 * Date: 03/13/2020
 * Description: the audio layer, which handles the movement as well.
 * References: 
	- https://github.com/eppz/Unity.Library.eppz_easing
*/

#pragma once

#include "florp/app/ApplicationLayer.h"
#include "GLM/vec3.hpp"
#include <string>

class AudioLayer : public florp::app::ApplicationLayer
{
public:
	void Initialize() override;
	
	void Shutdown() override;
	
	// lerp
	static float Lerp(float a, float b, float u);

	// ease in interpolation (see reference)
	static float EaseInLerp(float a, float b, float u);

	// ease out interpolation (see reference)
	static float EaseOutLerp(float a, float b, float u);

	// ease in and out
	static float EaseInOutLerp(float a, float b, float u);

	void Update() override;

private:
	// TODO: add play button for sound.
	std::string audioEvent = "Car Crash"; // the event for the sound
	float elapsedTime = 0.0F;
	bool playing = false;

	// x is left/right, y is up/down, z is in/out of the screen.
	// this is assuming that the camera orientation is its default, which it may not be.

	// start, end, and current position
	glm::vec3 startPos = glm::vec3(-1.0F, 0.0F, 0.0F);
	glm::vec3 endPos = glm::vec3(15.0F, 0.0F, 0.0F);
	glm::vec3 currPos = glm::vec3();

	// 'u' value for lerp.
	float u = 0.0F;

	// incrementer
	const float U_INC = 0.09F;

protected:

};
