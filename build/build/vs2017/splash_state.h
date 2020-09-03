#pragma once
#include "base_state.h"

class splash_state : public base_state
{
public:
	splash_state(gef::Platform* platform, gef::SpriteRenderer& renderer);
	~splash_state();

	// Default
	void Init() override;
	void CleanUp();
	void HandleInput(const gef::SonyController* controller) override;
	void Render() override;
};

