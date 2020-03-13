#pragma once
#include "florp/game/IBehaviour.h"
#include <GLM/glm.hpp>
#include "florp/game/Transform.h"
#include "florp/game/SceneManager.h"
#include "florp/app/Timing.h"
#include "florp/app/Window.h"
#include "florp/app/Application.h"
#include "AudioEngine.h"


class ListenerBehaviour : public florp::game::IBehaviour {
public:
	ListenerBehaviour() : IBehaviour(){};
	virtual ~ListenerBehaviour() = default;

	virtual void OnLoad(entt::entity entity) override {
		auto& transform = CurrentRegistry().get<florp::game::Transform>(entity);
		
	}

	virtual void Update(entt::entity entity) override {
		using namespace florp::app;
		auto& transform = CurrentRegistry().get<florp::game::Transform>(entity);

		AudioEngine& audioEngine = AudioEngine::GetInstance();

		// Set listener position
		audioEngine.SetListenerPosition(transform.GetLocalPosition());

		// Set listener orientation
		audioEngine.SetListenerOrientation(transform.GetUp(), transform.GetForward());

	}

};