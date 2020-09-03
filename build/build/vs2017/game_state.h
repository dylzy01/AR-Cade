#pragma once
// Sony
#include <sony_sample_framework.h>
#include <sony_tracking.h>
#include <platform/vita/graphics/texture_vita.h>
// GEF
#include <graphics/render_target.h>
#include <graphics/image_data.h>
#include <graphics/renderer_3d.h>
// System
#include <ctime>
#include <random>
#include <algorithm>
// Custom
#include "base_state.h"
#include "player_object.h"
#include "orbit_object.h"
#include "enemy_object.h"
#include "collision.h"

class game_state : public base_state
{
public:
	game_state(gef::Platform* platform, gef::SpriteRenderer& rendererSprite, gef::Renderer3D& renderer3d, PrimitiveBuilder& builder);
	~game_state();

	// Default //
	void Init() override;
	void CleanUp();
	void Update(float deltaTime) override;
	void HandleInput(const gef::SonyController* controller) override;
	void Render() override;

private:
	// Enemy management //
	void DefineMaxEnemies();
	void SetupEnemies();
	float CalculateDistance(game_object* one, game_object* two);
	float RandomSpawnX(game_object* one, game_object* two);
	float RandomSpawnY();
	int RandomDirection();

	// Game management //
	void EndGame(ENDGAME endgame);
	void ResetGame();
	void ResetLights();
	void CheckCollisions();

	// Materials & textures //
	void InitMaterials();
	void InitTextures();
	void RenderOverlay();

private:
	// Camera feed //
	// Orthographic projection matrix
	gef::Matrix44 m_orthographicMatrix;
	// Image scale factor / matrix
	float m_cameraImageAspectRatio, m_displayAspectRatio, m_scaleFactorY;
	const float m_cameraImageWidth = 640.f, m_cameraImageHeight = 480.f;
	const float m_displayWidth = 960.f, m_displayHeight = 544.f;
	// Sprite for drawing the camera feed
	gef::Sprite m_cameraFeedSprite;
	// Texture map for camera feed
	gef::TextureVita m_cameraFeedTexture;

	// 3D elements //
	// 3D projection matrix
	gef::Matrix44 m_projectionMatrixFOV, m_projectionMatrixFINAL;
	// Scale matrix
	gef::Matrix44 m_scaleMatrix;
	// 3D renderer
	class gef::Renderer3D* m_renderer3d;
	
private:
	// FPS
	float m_deltaTime;

	// Collision detection
	collision m_collisionChecker;
	
	// Materials
	gef::Material m_matRed, m_matBlue, m_matGreen, m_matYellow, m_matPink;

	// Gameplay controllers
	bool m_beginMovement, m_shouldSpawnObjects, m_outOfRange, m_shouldSetupEnemies, m_isInRadius, m_clockwise;
	std::vector<bool> m_shouldBuildLevel;

	// Marker controllers
	std::vector<int> m_markerIds;
	std::vector<bool> m_markerFound;
	std::vector<gef::Matrix44> m_markerTransform;

	// Render controllers
	bool m_shouldDrawPlayer;
	std::vector<bool> m_shouldDrawOrbits, m_shouldDrawEnemies, m_shouldDrawStartFinish;

	// Game objects
	player_object* m_player;
	std::vector<orbit_object*> m_orbits;
	std::vector<enemy_object*> m_enemies;
	std::vector<game_object*> m_startFinish;

	// Enemy management
	int m_maxEnemies;
	std::vector<gef::Vector4> m_enemyStartLocations;
};

