#include "ar_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/texture.h>
#include <graphics/mesh.h>
#include <graphics/primitive.h>
#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/font.h>
#include <input/touch_input_manager.h>
#include <maths/vector2.h>
#include <input/input_manager.h>
#include <maths/math_utils.h>
#include <graphics/renderer_3d.h>
#include <graphics/render_target.h>
#include <graphics/scene.h>
#include <sony_sample_framework.h>
#include <sony_tracking.h>

// Default //

ARApp::ARApp(gef::Platform& platform) :
	Application(platform),
	m_inputManager(NULL),
	m_spriteRenderer(NULL),
	m_font(NULL),
	m_renderer3d(NULL),
	m_primitiveBuilder(NULL)
{
	// Define start initialisation
	m_initialiseOnce = true;
	m_cleanUpOnce = true;
}

// Initialise States //

void ARApp::Init()
{
	// Initialise main app class
	if (m_initialiseOnce)
	{
		m_initialiseOnce = false;

		if (!m_inputManager) { m_inputManager = gef::InputManager::Create(platform_); }
		if (!m_spriteRenderer) { m_spriteRenderer = gef::SpriteRenderer::Create(platform_); }
		if (!m_renderer3d) { m_renderer3d = gef::Renderer3D::Create(platform_); }
		if (!m_primitiveBuilder) { m_primitiveBuilder = new PrimitiveBuilder(platform_); }

		// Define states
		m_splashState = new splash_state(&platform_, *m_spriteRenderer);
		m_howtoState = new howto_state(&platform_, *m_spriteRenderer);
		m_gameState = new game_state(&platform_, *m_spriteRenderer, *m_renderer3d, *m_primitiveBuilder);
		m_endGameState = new endgame_state(&platform_, *m_spriteRenderer);

		SetupLights();

		// Set next state
		m_currentState = STATE::SPLASH;
	}
	
	// Initialise per state
	switch (m_currentState)
	{
		case STATE::SPLASH:
		{
			m_splashState->Init();
			break;
		}
		case STATE::HOWTO:
		{
			m_howtoState->Init();
			break;
		}
		case STATE::GAME:
		{
			m_gameState->Init();
			break;
		}
		case STATE::ENDGAME:
		{
			m_endGameState->Init();
			break;
		}
	}
}

// Clean Up States //

void ARApp::CleanUp()
{
	// Clean up main app class
	if (m_cleanUpOnce)
	{
		m_cleanUpOnce = false;

		delete m_primitiveBuilder;
		m_primitiveBuilder = NULL;

		delete m_spriteRenderer;
		m_spriteRenderer = NULL;

		delete m_renderer3d;
		m_renderer3d = NULL;

		delete m_inputManager;
		m_inputManager = NULL;
	}
	
	// Clean up per state
	switch (m_currentState)
	{
		case STATE::SPLASH:
		{
			m_splashState->CleanUp();
			break;
		}
		case STATE::HOWTO:
		{
			m_howtoState->CleanUp();
			break;
		}
		case STATE::GAME:
		{
			m_gameState->CleanUp();
			break;
		}
		case STATE::ENDGAME:
		{
			m_endGameState->CleanUp();
			break;
		}
	}

	// Update current state
	UpdateCurrentState();
}

// Update States //

bool ARApp::Update(float deltaTime)
{
	// Define FPS
	m_fps = 1.0f / deltaTime;

	// Update game state if current state
	switch (m_currentState)
	{
		case STATE::GAME:
		{
			m_gameState->Update(deltaTime);
			m_endGameState->SetEndGameState(m_gameState->GetEndGameState());
			break;
		}
	}

	// Manage all input per state
	HandleInput();

	// Update current state
	UpdateCurrentState();
	
	// Break loop
	return true;
}

// Render States //

void ARApp::Render()
{
	// Render per state
	switch (m_currentState)
	{
		case STATE::SPLASH:
		{
			m_splashState->Render();
			break;
		}
		case STATE::HOWTO:
		{
			m_howtoState->Render();
			break;
		}
		case STATE::GAME:
		{
			m_gameState->Render();
			break;
		}
		case STATE::ENDGAME:
		{
			m_endGameState->Render();
			break;
		}
	}

	// Update current state
	UpdateCurrentState();
}

// Setup Camera Lights //

void ARApp::SetupLights()
{
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
	default_point_light.set_position(gef::Vector4(-300.0f, -500.0f, 100.0f));

	gef::Default3DShaderData& default_shader_data = m_renderer3d->default_shader_data();
	default_shader_data.set_ambient_light_colour(gef::Colour(0.5f, 0.5f, 0.5f, 1.0f));
	default_shader_data.AddPointLight(default_point_light);
}

// Handle Inputs per State

void ARApp::HandleInput()
{
	// Get latest data from input devices
	m_inputManager->Update();
	
	// Get controller data for all contollers
	gef::SonyControllerInputManager* controllerManager = m_inputManager->controller_input();

	if (controllerManager)
	{
		// Return controller data in a read-only format (const)
		// Read controller data for controller 0
		const gef::SonyController* controller = controllerManager->GetController(0);

		if (controller)
		{
			// Handle input per state
			switch (m_currentState)
			{
				case STATE::SPLASH:
				{
					m_splashState->HandleInput(controller);
					break;
				}
				case STATE::HOWTO:
				{
					m_howtoState->HandleInput(controller);
					break;
				}
				case STATE::GAME:
				{
					m_gameState->HandleInput(controller);
					break;
				}
				case STATE::ENDGAME:
				{
					m_endGameState->HandleInput(controller);
					break;
				}
			}
		}
	}
}

// Current State //

void ARApp::UpdateCurrentState()
{
	// Update current state depending on current local state per state
	switch (m_currentState)
	{
		case STATE::SPLASH:
		{
			if (m_currentState != m_splashState->GetState())
			{
				SetCurrentState(m_splashState->GetState());
				Init();
			}
			break;
		}
		case STATE::HOWTO:
		{
			if (m_currentState != m_howtoState->GetState())
			{
				SetCurrentState(m_howtoState->GetState());
				Init();
			}
			break;
		}
		case STATE::GAME:
		{
			if (m_currentState != m_gameState->GetState())
			{
				SetCurrentState(m_gameState->GetState());
				Init();
			}
			break;
		}
		case STATE::ENDGAME:
		{
			if (m_currentState != m_endGameState->GetState())
			{
				///SetCurrentState(m_loseState->GetState());
				SetCurrentState(m_endGameState->GetState());
				Init();
			}
		}
	}
}

void ARApp::SetCurrentState(STATE state)
{
	m_currentState = state;
}
