#include "splash_state.h"



splash_state::splash_state(gef::Platform* platform, gef::SpriteRenderer& renderer)
{
	m_platform = platform;
	m_spriteRenderer = &renderer;
}

splash_state::~splash_state()
{
	
}

void splash_state::Init()
{
	// Define state
	m_state = STATE::SPLASH;
	
	// Define background
	m_textures.push_back(m_empty);
	m_textures[0] = CreateTextureFromPNG("splash.png", *m_platform);
}

void splash_state::CleanUp()
{
	// Textures
	base_state::CleanUp();
}

void splash_state::HandleInput(const gef::SonyController * controller)
{
	// Cross button = game state
	if (controller->buttons_pressed() == gef_SONY_CTRL_CROSS)
	{
		// Release resources
		CleanUp();

		m_state = STATE::GAME;
	}

	// Cross button = how to state
	if (controller->buttons_pressed() == gef_SONY_CTRL_SQUARE)
	{
		// Release resources
		CleanUp();

		m_state = STATE::HOWTO;
	}
}

void splash_state::Render()
{
	// Setup render overlay
	gef::Matrix44 projectionMatrix2D;
	projectionMatrix2D = m_platform->OrthographicFrustum(0.0f, m_platform->width(), 0.0f, m_platform->height(), -1.0f, 1.0f);
	m_spriteRenderer->set_projection_matrix(projectionMatrix2D);
	m_spriteRenderer->Begin(false);

	// Setup background sprite
	gef::Sprite sprSplash;
	sprSplash.set_texture(m_textures[0]);
	sprSplash.set_position(gef::Vector4(m_platform->width() / 2, m_platform->height() / 2, -0.99f));
	sprSplash.set_width(m_platform->width());
	sprSplash.set_height(m_platform->height());
	m_spriteRenderer->DrawSprite(sprSplash);

	// End renderering
	m_spriteRenderer->End();
}

