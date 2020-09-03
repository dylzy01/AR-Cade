#include "game_state.h"

game_state::game_state(gef::Platform* platform, gef::SpriteRenderer& rendererSprite, gef::Renderer3D& renderer3d, PrimitiveBuilder& builder)
{
	m_platform = platform;
	m_spriteRenderer = &rendererSprite;
	m_renderer3d = &renderer3d;
	m_primitiveBuilder = &builder;
}

game_state::~game_state()
{
}

void game_state::Init()
{
	// Define state
	m_state = STATE::GAME;

	// Setup materials & textures
	InitTextures();
	InitMaterials();

	// Initialise sony framework
	sampleInitialize();
	smartInitialize();

	// Reset marker tracking
	AppData* dat = sampleUpdateBegin();
	smartTrackingReset();
	sampleUpdateEnd(dat);

	// Camera feed
	{
		// Orthographic Projection Matrix
		m_orthographicMatrix = m_platform->OrthographicFrustum(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);
		// Image Scale Factor
		m_cameraImageAspectRatio = float(SCE_SMART_IMAGE_WIDTH) / float(SCE_SMART_IMAGE_HEIGHT);
		m_displayAspectRatio = m_displayWidth / m_displayHeight;
		m_scaleFactorY = m_displayAspectRatio / m_cameraImageAspectRatio;
		// Sprite for drawing the camera feed
		{
			// Setting the width and height of the sprite to avoid distortion of camera feed image
			m_cameraFeedSprite.set_width(2.f);
			m_cameraFeedSprite.set_height(2.f * m_scaleFactorY);
			// Position the sprite at the very back of the projection space
			m_cameraFeedSprite.set_position(0.f, 0.f, 1.f);
			// Apply camera feed texture to sprite
			m_cameraFeedSprite.set_texture(&m_cameraFeedTexture);
		}
	}

	// 3D elements
	{
		// 3D projection matrix
		m_projectionMatrixFOV = m_platform->PerspectiveProjectionFov(SCE_SMART_IMAGE_FOV, (float(SCE_SMART_IMAGE_WIDTH) / float(SCE_SMART_IMAGE_HEIGHT)), 0.01, 100.0);
		m_scaleMatrix.Scale(gef::Vector4(1, m_scaleFactorY, 1));
		m_projectionMatrixFINAL = m_projectionMatrixFOV * m_scaleMatrix;
	}

	// Use current time as seed for random generator
	std::srand(std::time(nullptr));

	// Player shouldn't move
	m_beginMovement = false;

	// Initialise markers
	{
		gef::Matrix44 empty;
		empty.SetIdentity();
		for (int i = 0; i < 6; i++)
		{
			m_markerIds.push_back(i);
			m_markerFound.push_back(false);
			m_markerTransform.push_back(empty);
		}
	}

	// Initialise game objects
	{
		// Objects shouldn't be spawned & m_outOfRange is invalid
		{
			m_shouldSpawnObjects = false;
			m_outOfRange = false;
		}

		// Player
		{
			m_player = new player_object(m_primitiveBuilder);
			m_player->SetMaterial(&m_matBlue);
			m_shouldDrawPlayer = false;
		}

		// Orbits
		{
			for (int i = 0; i < 4; i++)
			{
				m_orbits.push_back(new orbit_object(m_primitiveBuilder));
				m_orbits[i]->SetMaterial(&m_matPink);
				m_orbits[i]->SetMaterialRadius(&m_matYellow);
				m_shouldDrawOrbits.push_back(false);
			}
		}

		// Enemies
		{
			m_shouldSetupEnemies = true;
			m_maxEnemies = 0;
		}

		// Start & Finish
		{
			for (int i = 0; i < 2; i++)
			{
				m_startFinish.push_back(new game_object());
				m_startFinish[i]->set_mesh(m_primitiveBuilder->CreateSphereMesh(1.0f, 10.f, 10.f, gef::Vector4(0, 0, 0), &m_matGreen));
				m_startFinish[i]->SetScale(gef::Vector4(0.059, 0.059, 0.059));
				m_shouldDrawStartFinish.push_back(false);
			}
		}
	}

	// Initialise level building controller
	{
		for (int i = 0; i < 2; i++)
		{
			m_shouldBuildLevel.push_back(false);
		}
	}
}
 
void game_state::CleanUp()
{
	// Textures
	base_state::CleanUp();

	// Player
	{
		delete m_player;
		m_player = NULL;
	}

	// Orbits
	{
		for (int i = 0; i < m_orbits.size(); i++)
		{
			delete m_orbits[i];
			m_orbits[i] = NULL;
		}
		m_orbits.clear();
	}

	// Enemies
	{
		for (int i = 0; i < m_enemies.size(); i++)
		{
			delete m_enemies[i];
			m_enemies[i] = NULL;
		}
		m_enemies.clear();
	}

	// Start & Finish
	{
		for (int i = 0; i < m_startFinish.size(); i++)
		{
			delete m_startFinish[i];
			m_startFinish[i] = NULL;
		}
		m_startFinish.clear();
	}

	// AR
	smartRelease();
	sampleRelease();
}

void game_state::Update(float deltaTime)
{
	// Define FPS
	m_deltaTime = deltaTime;

	// Begin sampling marker data
	AppData* dat = sampleUpdateBegin();

	// Use the tracking library to try and find markers
	smartUpdate(dat->currentImage);

	// Check to see if markers are found
	for (int i = 0; i < m_markerIds.size(); i++)
	{
		// If marker is found...
		if (sampleIsMarkerFound(m_markerIds[i]))
		{
			// Marker is being tracked, grab its transform!
			gef::Matrix44 markerTransform;
			sampleGetTransform(m_markerIds[i], &markerTransform);

			// Save the marker transform
			m_markerTransform[i] = markerTransform;

			// Marker has been found
			m_markerFound[i] = true;
		}
		else
		{
			m_markerFound[i] = false;
		}
	}

	// Check start/finish markers are in range
	if (m_markerFound[0] && m_markerFound[1])
	{
		m_outOfRange = false;
	}

	// Stop using the tracking library
	sampleUpdateEnd(dat);

	// If the start/finish markers are in range...
	if (!m_outOfRange)
	{
		// Marker 1
		if (m_markerFound[0])
		{
			// Should the object be drawn?
			m_shouldDrawStartFinish[0] = true;

			// Update the start transform to equal the marker transform
			m_startFinish[0]->UpdateTransform(m_markerTransform[0]);

			// Should the level be built?
			m_shouldBuildLevel[0] = true;
		}
		else
		{
			// Should the object be drawn?
			m_shouldDrawStartFinish[0] = false;

			// Should the level be built?
			m_shouldBuildLevel[0] = false;
		}

		// Marker 2
		if (m_markerFound[1])
		{
			// Should the object be drawn?
			m_shouldDrawStartFinish[1] = true;

			// Update the finish transform to equal the marker transform
			m_startFinish[1]->UpdateTransform(m_markerTransform[1]);

			// Should the level be built?
			m_shouldBuildLevel[1] = true;
		}
		else
		{
			// Should the object be drawn?
			m_shouldDrawStartFinish[1] = false;

			// Should the level be built?
			m_shouldBuildLevel[1] = false;
		}

		// Marker 3
		if (m_markerFound[2])
		{
			// Should the object be drawn?
			m_shouldDrawOrbits[0] = true;

			// Update the object transform to equal the marker transform
			m_orbits[0]->UpdateTransform(m_markerTransform[2]);
			m_orbits[0]->GetRadius()->UpdateTransform(m_markerTransform[2]);
		}
		else
		{
			// Should the object be drawn?
			m_shouldDrawOrbits[0] = false;
		}

		// Marker 4
		if (m_markerFound[3])
		{
			// Should the object be drawn?
			m_shouldDrawOrbits[1] = true;

			// Update the object transform to equal the marker transform
			m_orbits[1]->UpdateTransform(m_markerTransform[3]);
			m_orbits[1]->GetRadius()->UpdateTransform(m_markerTransform[3]);
		}
		else
		{
			// Should the object be drawn?
			m_shouldDrawOrbits[1] = false;
		}

		// Marker 5
		if (m_markerFound[4])
		{
			// Should the object be drawn?
			m_shouldDrawOrbits[2] = true;

			// Update the object transform to equal the marker transform
			m_orbits[2]->UpdateTransform(m_markerTransform[4]);
			m_orbits[2]->GetRadius()->UpdateTransform(m_markerTransform[4]);
		}
		else
		{
			// Should the object be drawn?
			m_shouldDrawOrbits[2] = false;
		}

		// Marker 6
		if (m_markerFound[5])
		{
			// Should the object be drawn?
			m_shouldDrawOrbits[3] = true;

			// Update the object transform to equal the marker transform
			m_orbits[3]->UpdateTransform(m_markerTransform[5]);
			m_orbits[3]->GetRadius()->UpdateTransform(m_markerTransform[5]);
		}
		else
		{
			// Should the object be drawn?
			m_shouldDrawOrbits[3] = false;
		}

		// Build the level
		if (m_shouldSpawnObjects)
		{
			if (m_shouldBuildLevel[0] && m_shouldBuildLevel[1])
			{
				// Player //

				// Should the object be drawn ?
				m_shouldDrawPlayer = true;

				// Should the player begin movement ?
				if (m_beginMovement)
				{
					m_player->Update(deltaTime);
				}

				// Update the player transform to equal the marker transform
				m_player->UpdateTransform(m_markerTransform[0]);

				// Enemies //

				// Should the object(s) be drawn ?
				for (int i = 0; i < m_enemies.size(); i++)
				{
					m_shouldDrawEnemies[i] = true;
				}

				// Define enemy starting positions
				if (m_shouldSetupEnemies)
				{
					m_shouldSetupEnemies = false;

					DefineMaxEnemies();
					SetupEnemies();

					for (int i = 0; i < m_enemies.size(); i++)
					{
						m_enemies[i]->SetPosition(m_enemyStartLocations[i]);
						m_enemies[i]->GetRadius()->SetPosition(m_enemyStartLocations[i]);
					}
				}

				// Enemy transforms
				for (int i = 0; i < m_enemies.size(); i++)
				{
					m_enemies[i]->Update();

					m_enemies[i]->UpdateTransform(m_markerTransform[0]);
					m_enemies[i]->GetRadius()->UpdateTransform(m_markerTransform[0]);
				}
			}
			else
			{
				// Pause & display error message
				m_outOfRange = true;

				// Should the object be drawn ?
				m_shouldDrawPlayer = false;

				// Should the object(s) be drawn ?
				for (int i = 0; i < m_enemies.size(); i++)
				{
					m_shouldDrawEnemies[i] = false;
				}
			}
		}
	}

	// Check for all collisions
	CheckCollisions();
	
}

void game_state::HandleInput(const gef::SonyController * controller)
{
	if (controller->buttons_pressed() == gef_SONY_CTRL_CROSS)
	{
		if (!m_shouldSpawnObjects)
		{
			m_shouldSpawnObjects = true;
		}
		else
		{
			if (!m_beginMovement)
			{
				m_beginMovement = true;
			}
			else
			{
				if (!m_player->GetLockOn())
				{
					// Begin orbit
					m_player->SetLockOn(true);
				}
				else
				{
					// End orbit
					m_player->SetLockOn(false);
					m_player->SetShouldOrbit(false);
					m_player->SetTargetOrbit(-1);
				}
			}
		}
	}
	if (controller->buttons_pressed() == gef_SONY_CTRL_SQUARE)
	{
		if (!m_clockwise)
		{
			// Orbit clockwise
			m_clockwise = true;
		}
		else
		{
			// Orbit anti-clockwise
			m_clockwise = false;
		}
	}
	if (controller->buttons_pressed() == gef_SONY_CTRL_START)
	{
		ResetGame();
	}
	if (controller->buttons_down() == gef_SONY_CTRL_L2)
	{
		m_player->SetTurn(-75.f);
	}
	if (controller->buttons_down() == gef_SONY_CTRL_R2)
	{
		m_player->SetTurn(75.f);
	}
	if (controller->buttons_pressed() == gef_SONY_CTRL_TRIANGLE)
	{
		ResetLights();
	}
}

void game_state::Render()
{
	AppData* dat = sampleRenderBegin();

	//
	// Render the camera feed
	//

	// Applying the Orthographic Matrix
	m_spriteRenderer->set_projection_matrix(m_orthographicMatrix);

	m_spriteRenderer->Begin(true);

	// Check there's data present for the camera image before trying to draw it
	if (dat->currentImage)
	{
		m_cameraFeedTexture.set_texture(dat->currentImage->tex_yuv);
		m_spriteRenderer->DrawSprite(m_cameraFeedSprite);
	}

	m_spriteRenderer->End();

	//
	// Render 3D scene
	//

	// Applying the modified Projection Matrix
	m_renderer3d->set_projection_matrix(m_projectionMatrixFINAL);

	// Applying a View Matrix
	gef::Matrix44 identityMatrix;
	identityMatrix.SetIdentity();
	m_renderer3d->set_view_matrix(identityMatrix);

	// Begin rendering 3D meshes, don't clear the frame buffer
	m_renderer3d->Begin(false);

	// Should the objects be spawned ?
	if (m_shouldSpawnObjects)
	{
		// Should the player be drawn ?
		if (m_shouldDrawPlayer)
		{
			m_renderer3d->DrawMesh(*m_player);
		}

		// Should the enemies be drawn ?
		for (int i = 0; i < m_enemies.size(); i++)
		{
			if (m_shouldDrawEnemies[i])
			{
				m_renderer3d->DrawMesh(*m_enemies[i]);
			}
		}

		// Should the orbit(s) be drawn?
		for (int i = 0; i < m_shouldDrawOrbits.size(); i++)
		{
			if (m_shouldDrawOrbits[i])
			{
				m_renderer3d->DrawMesh(*m_orbits[i]);
				m_renderer3d->DrawMesh(*m_orbits[i]->GetRadius());
			}
		}
	}

	// Should the start/finish be drawn?
	for (int i = 0; i < m_shouldDrawStartFinish.size(); i++)
	{
		if (m_shouldDrawStartFinish[i])
		{
			m_renderer3d->DrawMesh(*m_startFinish[i]);
		}
	}

	m_renderer3d->End();

	RenderOverlay();

	sampleRenderEnd();
}

void game_state::DefineMaxEnemies()
{
	float dist = CalculateDistance(m_startFinish[0], m_startFinish[1]);

	if (dist <= 0.15)
	{
		m_maxEnemies = 1;
	}
	else if (dist <= 0.25)
	{
		m_maxEnemies = 2;
	}
	else if (dist <= 0.35)
	{
		m_maxEnemies = 3;
	}
	else if (dist <= 0.45)
	{
		m_maxEnemies = 4;
	}
	else if (dist <= 0.55)
	{
		m_maxEnemies = 5;
	}
	else if (dist <= 0.65)
	{
		m_maxEnemies = 6;
	}
	else if (dist <= 0.75)
	{
		m_maxEnemies = 7;
	}
	else if (dist <= 0.85)
	{
		m_maxEnemies = 8;
	}
	else if (dist <= 0.95)
	{
		m_maxEnemies = 9;
	}
	else
	{
		m_maxEnemies = 10;
	}
}

void game_state::SetupEnemies()
{
	for (int i = 0; i < m_maxEnemies; i++)
	{
		// Enemy objects
		m_enemies.push_back(new enemy_object(m_primitiveBuilder, RandomDirection()));
		m_enemies[i]->SetMaterial(&m_matRed);

		// Enemy controllers
		m_shouldDrawEnemies.push_back(false);
		m_enemies[i]->SetHasCollided(false);

		// Random spawn positions
		m_enemyStartLocations.push_back(gef::Vector4());
		m_enemyStartLocations[i] = gef::Vector4(RandomSpawnX(m_startFinish[0], m_startFinish[1]), RandomSpawnY(), 0);
	}
}

float game_state::CalculateDistance(game_object * one, game_object * two)
{
	float X1, X2, Y1, Y2;
	X1 = one->GetWorldTransform().GetTranslation().x();		Y1 = one->GetWorldTransform().GetTranslation().y();
	X2 = two->GetWorldTransform().GetTranslation().x();		Y2 = two->GetWorldTransform().GetTranslation().y();

	float distX, distY;
	distX = (X2 - X1) * (X2 - X1);
	distY = (Y2 - Y1) * (Y2 - Y1);

	return sqrt(distX + distY);
}

float game_state::RandomSpawnX(game_object * one, game_object * two)
{
	// source: https://stackoverflow.com/questions/53040940/why-is-the-new-random-library-better-than-stdrand

	// Store X positions from both objects
	float X1, X2;
	X1 = one->GetWorldTransform().GetTranslation().x();
	X2 = two->GetWorldTransform().GetTranslation().x();

	// Generate a random number between (X1 & X2)
	std::random_device randomDevice;
	std::mt19937 mt(randomDevice());
	std::uniform_real_distribution<> distributor(X1, X2);

	// Return random X position
	float random = distributor(mt);
	return random + 0.25;
}

float game_state::RandomSpawnY()
{
	// source: https://stackoverflow.com/questions/53040940/why-is-the-new-random-library-better-than-stdrand

	// Generate a random number between (-5 & 5)
	std::random_device randomDevice;
	std::mt19937 mt(randomDevice());
	std::uniform_real_distribution<> distributor(-0.1, 0.1);

	// Return random Y position
	float random = distributor(mt);
	return random;
}

int game_state::RandomDirection()
{
	// source: https://stackoverflow.com/questions/53040940/why-is-the-new-random-library-better-than-stdrand

	// Generate a random number between (1 & 4)
	std::random_device randomDevice;
	std::mt19937 mt(randomDevice());
	std::uniform_real_distribution<> distributor(1, 4);

	// Return random X position
	int random = distributor(mt);
	return random;
}

void game_state::EndGame(ENDGAME endgame)
{
	CleanUp();
	
	m_endgameState = endgame;
	m_state = STATE::ENDGAME;
}

void game_state::ResetGame()
{
	// Release game resources
	CleanUp();

	// Initialise game
	Init();
}

void game_state::ResetLights()
{
	// release sony framework
	smartRelease();
	sampleRelease();

	// initialise sony framework
	sampleInitialize();
	smartInitialize();

	// reset marker tracking
	AppData* dat = sampleUpdateBegin();
	smartTrackingReset();
	sampleUpdateEnd(dat);
}

void game_state::CheckCollisions()
{
	// Collisions during gameplay
	if (m_beginMovement)
	{
		// Does the player want to orbit ?
		if (m_player->GetLockOn())
		{
			// Loop through available orbits
			for (int i = 0; i < m_orbits.size(); i++)
			{
				// Is the player in an orbit radius ?
				if (m_collisionChecker.CollisionSphere(m_player, m_orbits[i]->GetRadius()))
				{
					// Assign target orbit to currently colliding orbit
					m_player->SetTargetOrbit(i);

					m_isInRadius = true;
				}
			}
		}
		else
		{
			m_isInRadius = false;
		}

		// Does the player have a target orbit ?
		if (m_player->GetTargetOrbit() != -1)
		{
			// Is the player within the target orbit radius ?
			if (m_collisionChecker.CollisionSphere(m_player, m_orbits[m_player->GetTargetOrbit()]->GetRadius()))
			{
				// Tell the player to begin oribt
				m_player->SetShouldOrbit(true);
			}
		}

		// Has the player reached the end ?
		if (m_collisionChecker.CollisionSphere(m_player, m_startFinish[1]))
		{
			// Player wins! 
			EndGame(ENDGAME::WIN);
		}

		// Check for player collisions with enemies
		for (int i = 0; i < m_enemies.size(); i++)
		{
			// Is the player colliding with an enemy and is not immune ?
			if (m_collisionChecker.CollisionSphere(m_player, m_enemies[i]) && m_player->GetCanDie())
			{
				// Player is immune for 3 seconds
				m_player->SetCanDie(false);

				// Decrement total lives
				m_player->DecrementLives();

				// If 3 lives are gone, end game
				if (m_player->GetLives() <= 0)
				{
					// Player loses!
					EndGame(ENDGAME::LOSE);
				}
			}
		}
	}

	// Loop through available enemies
	for (int i = 0; i < m_enemies.size(); i++)
	{
		// Has the enemy left their orbit radius ?
		if (m_collisionChecker.CollisionSphere(m_enemies[i], m_enemies[i]->GetRadius())) {}
		else
		{
			// Reverse current enemy movement direction
			m_enemies[i]->Reverse();
		}
	}
}

void game_state::InitMaterials()
{
	m_matRed.set_colour(0xFF3440EB);
	m_matBlue.set_colour(0xFFBE9B34);
	m_matGreen.set_colour(0x507AEB34);
	m_matYellow.set_colour(0x5000FBFF);
	m_matPink.set_colour(0xFFFF00FF);
}

void game_state::InitTextures()
{
	// HUD - Lives
	for (int i = 0; i < 4; i++) { m_textures.push_back(m_empty); }
	m_textures[0] = CreateTextureFromPNG("lives-0.png", *m_platform);
	m_textures[1] = CreateTextureFromPNG("lives-1.png", *m_platform);
	m_textures[2] = CreateTextureFromPNG("lives-2.png", *m_platform);
	m_textures[3] = CreateTextureFromPNG("lives-3.png", *m_platform);

	// HUD - Direction
	for (int i = 0; i < 2; i++) { m_textures.push_back(m_empty); }
	m_textures[4] = CreateTextureFromPNG("arrow-left.png", *m_platform);
	m_textures[5] = CreateTextureFromPNG("arrow-right.png", *m_platform);

	// HUD - Orbit on/off
	for (int i = 0; i < 2; i++) { m_textures.push_back(m_empty); }
	m_textures[6] = CreateTextureFromPNG("orbit-off.png", *m_platform);
	m_textures[7] = CreateTextureFromPNG("orbit-on.png", *m_platform);

	// Prompt - Spawn objects
	m_textures.push_back(m_empty);
	m_textures[8] = CreateTextureFromPNG("prompt-spawn.png", *m_platform);

	// Prompt - Press X to begin movement
	m_textures.push_back(m_empty);
	m_textures[9] = CreateTextureFromPNG("prompt-movement.png", *m_platform);

	// Prompt - Return objects to view space
	m_textures.push_back(m_empty);
	m_textures[10] = CreateTextureFromPNG("prompt-return.png", *m_platform);

	// Prompt - Press X to lock on to orbit
	m_textures.push_back(m_empty);
	m_textures[11] = CreateTextureFromPNG("prompt-lockon.png", *m_platform);

	// Prompt - Press X to release from orbit
	m_textures.push_back(m_empty);
	m_textures[12] = CreateTextureFromPNG("prompt-release.png", *m_platform);
}

void game_state::RenderOverlay()
{
	// Setup render overlay
	gef::Matrix44 projectionMatrix2D;
	projectionMatrix2D = m_platform->OrthographicFrustum(0.0f, m_platform->width(), 0.0f, m_platform->height(), -1.0f, 1.0f);
	m_spriteRenderer->set_projection_matrix(projectionMatrix2D);
	m_spriteRenderer->Begin(false);

	// Lives sprite
	{
		gef::Sprite sprLives;
		if (m_player->GetLives() <= 0) { sprLives.set_texture(m_textures[0]); }
		else if (m_player->GetLives() == 1) { sprLives.set_texture(m_textures[1]); }
		else if (m_player->GetLives() == 2) { sprLives.set_texture(m_textures[2]); }
		else if (m_player->GetLives() == 3) { sprLives.set_texture(m_textures[3]); }
		sprLives.set_position(gef::Vector4(m_platform->width() / 2, m_platform->height() / 2, -0.99f));
		sprLives.set_width(m_platform->width());
		sprLives.set_height(m_platform->height());
		m_spriteRenderer->DrawSprite(sprLives);
	}

	// Direction sprite
	{
		gef::Sprite sprArrow;
		if (m_player->GetTurn() == -75.f) { sprArrow.set_texture(m_textures[4]); }
		else if (m_player->GetTurn() == 75.f) { sprArrow.set_texture(m_textures[5]); }
		sprArrow.set_position(gef::Vector4(m_platform->width() / 2, m_platform->height() / 2, -0.99f));
		sprArrow.set_width(m_platform->width());
		sprArrow.set_height(m_platform->height());
		m_spriteRenderer->DrawSprite(sprArrow);
	}

	// Orbit on/off sprite
	{
		gef::Sprite sprOrbit;
		if (!m_player->GetLockOn()) { sprOrbit.set_texture(m_textures[6]); }
		else if (m_player->GetLockOn()) { sprOrbit.set_texture(m_textures[7]); }
		sprOrbit.set_position(gef::Vector4(m_platform->width() / 2, m_platform->height() / 2, -0.99f));
		sprOrbit.set_width(m_platform->width());
		sprOrbit.set_height(m_platform->height());
		m_spriteRenderer->DrawSprite(sprOrbit);
	}

	// Spawn object prompt sprite
	{
		if (!m_shouldSpawnObjects)
		{
			gef::Sprite sprSpawnObjects;
			sprSpawnObjects.set_texture(m_textures[8]);
			sprSpawnObjects.set_position(gef::Vector4(m_platform->width() / 2, m_platform->height() / 2, -0.99f));
			sprSpawnObjects.set_width(m_platform->width());
			sprSpawnObjects.set_height(m_platform->height());
			m_spriteRenderer->DrawSprite(sprSpawnObjects);
		}
	}

	// Begin movement prompt sprite
	{
		if (m_shouldSpawnObjects && !m_beginMovement)
		{
			gef::Sprite sprBeginMovement;
			sprBeginMovement.set_texture(m_textures[9]);
			sprBeginMovement.set_position(gef::Vector4(m_platform->width() / 2, m_platform->height() / 2, -0.99f));
			sprBeginMovement.set_width(m_platform->width());
			sprBeginMovement.set_height(m_platform->height());
			m_spriteRenderer->DrawSprite(sprBeginMovement);
		}
	}

	// Return objects to view space prompt sprite
	if (m_shouldSpawnObjects && m_outOfRange)
	{
		gef::Sprite sprPause;
		sprPause.set_texture(m_textures[10]);
		sprPause.set_position(gef::Vector4(m_platform->width() / 2, m_platform->height() / 2, -0.99f));
		sprPause.set_width(m_platform->width());
		sprPause.set_height(m_platform->height());
		m_spriteRenderer->DrawSprite(sprPause);
	}

	if (m_beginMovement)
	{
		// Press X to lock on to orbit prompt sprite
		if (!m_player->GetLockOn())
		{
			gef::Sprite sprLockOn;
			sprLockOn.set_texture(m_textures[11]);
			sprLockOn.set_position(gef::Vector4(m_platform->width() / 2, m_platform->height() / 2, -0.99f));
			sprLockOn.set_width(m_platform->width());
			sprLockOn.set_height(m_platform->height());
			m_spriteRenderer->DrawSprite(sprLockOn);
		}
		// Press X to release from orbit
		else if (m_player->GetLockOn() && m_isInRadius)
		{
			gef::Sprite sprRelease;
			sprRelease.set_texture(m_textures[12]);
			sprRelease.set_position(gef::Vector4(m_platform->width() / 2, m_platform->height() / 2, -0.99f));
			sprRelease.set_width(m_platform->width());
			sprRelease.set_height(m_platform->height());
			m_spriteRenderer->DrawSprite(sprRelease);
		}
	}
	
	// End renderering
	m_spriteRenderer->End();
}
