#pragma once

#include "Engine/EngineState.h"
#include "GameplaySystem.h"

#include "cereal/cereal.hpp"
#include "cereal/archives/xml.hpp"

class GameState : public EngineState
{
public:
	explicit GameState(ENGINE_STATE_ID id) : EngineState(id) {}

	void init(std::string args = "") override;
	void update(float dt) override;
	void updateUI(float dt) override;
	void destroy() override;

	void pause() override;
	void resume() override;

    static void serializeComponent(anax::Entity& entity, cereal::XMLOutputArchive& archive);
    static void deserializeComponent(anax::Entity& entity, tinyxml2::XMLNode* component);
};
