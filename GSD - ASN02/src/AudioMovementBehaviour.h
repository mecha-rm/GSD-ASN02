#pragma once
#include "florp/game/IBehaviour.h"
#include <GLM/glm.hpp>
#include "florp/game/Transform.h"
#include "florp/game/SceneManager.h"
#include "florp/app/Timing.h"
#include "florp/app/Window.h"
#include "florp/app/Application.h"
#include "AudioEngine.h"


class AudioMovementBehaviour : public florp::game::IBehaviour {
public:
	AudioMovementBehaviour() : IBehaviour(), 
		newPosition(glm::vec3(0)), angle(4.7), angleIncrement(1), 
		radiusIncrement(5), radius(10),
		isAngleMoving(false), isRadiusMoving(false) {};
	virtual ~AudioMovementBehaviour() = default;

	virtual void OnLoad(entt::entity entity) override {
		auto& transform = CurrentRegistry().get<florp::game::Transform>(entity);
		// TODO:: ooo ooo ahh ahh
		AudioEngine& audioEngine = AudioEngine::GetInstance();

		audioEngine.LoadEvent("Monkey");
		audioEngine.PlayEvent("Monkey");

		newPosition = transform.GetLocalPosition();
	}

	virtual void Update(entt::entity entity) override {
		using namespace florp::app;
		auto& transform = CurrentRegistry().get<florp::game::Transform>(entity);
		Window::Sptr window = Application::Get()->GetWindow();

		
		// Input 
		if (window->IsKeyDown(Key::P)) {
			isAngleMoving = !isAngleMoving;
		}

		if (window->IsKeyDown(Key::O)) {
			isRadiusMoving = !isRadiusMoving;
		}
		
		if (window->IsKeyDown(Key::R)) {
			angle = 4.7;
			radius = 10;
		}

		// TODO: Move the monkey head in a circle
		if (isAngleMoving)
		{
			angle += angleIncrement * florp::app::Timing::DeltaTime;
		}

		// TODO: Move the monkey head forward and back
		if (isRadiusMoving)
		{
			// incrementing the radius.
			radius += radiusIncrement * florp::app::Timing::DeltaTime;

			// reversing the direction if the radius goes too far.
			if (radius < 10 || radius > 30)
			{
				radiusIncrement = -radiusIncrement;
			}
		}

		newPosition.x = cos(angle) * radius;
		newPosition.z = sin(angle) * radius;

		transform.SetPosition(newPosition);
		AudioEngine::GetInstance().SetEventPosition("Monkey", newPosition);
	}

private:
	glm::vec3 newPosition;
	float angle;
	float radius;
	float angleIncrement;
	float radiusIncrement;
	bool isAngleMoving;
	bool isRadiusMoving;

};