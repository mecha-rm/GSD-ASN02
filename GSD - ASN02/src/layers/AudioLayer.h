#pragma once
#include "florp/app/ApplicationLayer.h"
#include "GLM/vec3.hpp"

class AudioLayer : public florp::app::ApplicationLayer
{
public:
	void Initialize() override;
	void Shutdown() override;
	void Update() override;

private:
	// start, end, and current position
	glm::vec3 startPos;
	glm::vec3 endPos;
	glm::vec3 currPos;

	// 'u' value for lerp.
	float u_inc = 0.0F;

protected:

};
