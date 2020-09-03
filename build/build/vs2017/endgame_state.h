#pragma once
#include "base_state.h"

class endgame_state : public base_state
{
public:
	endgame_state(gef::Platform* platform, gef::SpriteRenderer& renderer);
	~endgame_state();

	// Default
	void Init() override;
	void CleanUp();
	void Update(float deltaTime) override;
	void HandleInput(const gef::SonyController* controller) override;
	void Render() override;
};