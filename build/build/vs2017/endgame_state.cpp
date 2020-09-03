#include "endgame_state.h"


endgame_state::endgame_state(gef::Platform * platform, gef::SpriteRenderer & renderer)
{
	m_platform = platform;
	m_spriteRenderer = &renderer;
}

endgame_state::~endgame_state()
{
}

void endgame_state::Init()
{
	// Define state
	m_state = STATE::ENDGAME;

	// Define background
	for (int i = 0; i < 2; i++)
	{
		m_textures.push_back(m_empty);
	}
	m_textures[0] = CreateTextureFromPNG("lose.png", *m_platform);
	m_textures[1] = CreateTextureFromPNG("win.png", *m_platform);
}

void endgame_state::CleanUp()
{
	// Textures
	base_state::CleanUp();
}

void endgame_state::Update(float deltaTime)
{
}

void endgame_state::HandleInput(const gef::SonyController * controller)
{
	if (controller->buttons_pressed() == gef_SONY_CTRL_CROSS)
	{
		// Release resources
		CleanUp();

		m_state = STATE::SPLASH;
	}
}

void endgame_state::Render()
{
	// Setup render overlay
	gef::Matrix44 projectionMatrix2D;
	projectionMatrix2D = m_platform->OrthographicFrustum(0.0f, m_platform->width(), 0.0f, m_platform->height(), -1.0f, 1.0f);
	m_spriteRenderer->set_projection_matrix(projectionMatrix2D);
	m_spriteRenderer->Begin(false);

	// Setup background sprite
	gef::Sprite sprLose;
	if (m_endgameState == ENDGAME::LOSE) { sprLose.set_texture(m_textures[0]); }
	else if (m_endgameState == ENDGAME::WIN) { sprLose.set_texture(m_textures[1]); }
	sprLose.set_position(gef::Vector4(m_platform->width() / 2, m_platform->height() / 2, -0.99f));
	sprLose.set_width(m_platform->width());
	sprLose.set_height(m_platform->height());
	m_spriteRenderer->DrawSprite(sprLose);

	// End renderering
	m_spriteRenderer->End();
}
