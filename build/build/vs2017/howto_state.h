#pragma once
#include "base_state.h"

class howto_state : public base_state
{
public:
	howto_state(gef::Platform* platform, gef::SpriteRenderer& renderer);
	~howto_state();

	// Default
	void Init() override;
	void CleanUp();
	void HandleInput(const gef::SonyController* controller) override;
	void Render() override;

private:
	// Controller
	int m_counter;
};

