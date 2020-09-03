#include "howto_state.h"



howto_state::howto_state(gef::Platform * platform, gef::SpriteRenderer& renderer)
{
	m_platform = platform;
	m_spriteRenderer = &renderer;
}

howto_state::~howto_state()
{

}

void howto_state::Init()
{
	// Define state
	m_state = STATE::HOWTO;
	
	// Define counter
	m_counter = 0;
	
	// Define backgrounds
	m_textures.clear();
	for (int i = 0; i < 9; i++)
	{
		m_textures.push_back(m_empty);
	}
	m_textures[0] = CreateTextureFromPNG("desc.png", *m_platform);
	m_textures[1] = CreateTextureFromPNG("key.png", *m_platform);
	m_textures[2] = CreateTextureFromPNG("controls-1.png", *m_platform);
	m_textures[3] = CreateTextureFromPNG("controls-2.png", *m_platform);
	m_textures[4] = CreateTextureFromPNG("controls-3.png", *m_platform);
	m_textures[5] = CreateTextureFromPNG("controls-4.png", *m_platform);
	m_textures[6] = CreateTextureFromPNG("controls-5.png", *m_platform);
	m_textures[7] = CreateTextureFromPNG("controls-6.png", *m_platform);
	m_textures[8] = CreateTextureFromPNG("controls-7.png", *m_platform);
}

void howto_state::CleanUp()
{
	// Textures
	base_state::CleanUp();
}

void howto_state::HandleInput(const gef::SonyController * controller)
{
	// Square button = next screen
	if (controller->buttons_pressed() == gef_SONY_CTRL_CROSS)
	{
		// Switch screen within the HOW TO state
		m_counter++;

		// If reached the final screen and button is still pressed
		if (m_counter > 8)
		{
			// Reset counter
			m_counter = 0;

			// Release HOW TO resources
			CleanUp();

			// Change the current state to the SPLASH state
			m_state = STATE::SPLASH;
		}
	}

	// Cross button = previous screen
	if (controller->buttons_pressed() == gef_SONY_CTRL_SQUARE)
	{
		// Switch screen within the HOW TO state
		m_counter--;

		// If reached the first screen and button is still pressed
		if (m_counter < 0)
		{
			// Reset counter
			m_counter = 0;

			// Release HOW TO resources
			CleanUp();

			// Change the current state to the SPLASH state
			m_state = STATE::SPLASH;
		}
	}
}

void howto_state::Render()
{
	// Setup render overlay
	gef::Matrix44 projectionMatrix2D;
	projectionMatrix2D = m_platform->OrthographicFrustum(0.0f, m_platform->width(), 0.0f, m_platform->height(), -1.0f, 1.0f);
	m_spriteRenderer->set_projection_matrix(projectionMatrix2D);
	m_spriteRenderer->Begin(false);

	// Setup background sprite
	gef::Sprite sprHowTo;
	sprHowTo.set_texture(m_textures[m_counter]);
	sprHowTo.set_position(gef::Vector4(m_platform->width() / 2, m_platform->height() / 2, -0.99f));
	sprHowTo.set_width(m_platform->width());
	sprHowTo.set_height(m_platform->height());
	m_spriteRenderer->DrawSprite(sprHowTo);

	// End renderering
	m_spriteRenderer->End();
}
