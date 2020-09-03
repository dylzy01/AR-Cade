#pragma once
// System
#include <vector>
// GEF
#include <gef.h>
#include <graphics/texture.h>
#include <graphics/sprite.h>
#include <graphics/sprite_renderer.h>
#include <graphics/primitive.h>
#include <graphics/renderer_3d.h>
#include <graphics/render_target.h>
#include <graphics/image_data.h>
#include <primitive_builder.h>
#include <gef_abertay/input/sony_controller_input_manager.h>
// Custom
#include "load_texture.h"
#include "states.h"

class base_state
{
public:
	base_state()	{};
	~base_state()	{};

	// Default
	virtual void Init()			{};
	virtual void CleanUp() 
	{
		for (int i = 0; i < m_textures.size(); i++)
		{
			delete m_textures[i];
			m_textures[i] = NULL;
		}
		m_textures.clear();
	};
	virtual void Update(float deltaTime)		{};
	virtual void HandleInput(const gef::SonyController* controller)	{};
	virtual void Render()		{};

	// Getters //
	STATE GetState() { return m_state; }
	ENDGAME GetEndGameState() { return m_endgameState; }

	// Setters //
	void SetEndGameState(ENDGAME endgame) { m_endgameState = endgame; }
	
protected:
	// Background
	std::vector<gef::Texture*> m_textures;
	gef::Texture* m_empty;

	// Local platform pointer
	gef::Platform* m_platform;

	// Sprite renderer pointer
	gef::SpriteRenderer* m_spriteRenderer;

	// Primitive builder pointer
	PrimitiveBuilder* m_primitiveBuilder;
	
	// Controller pointer
	gef::SonyController* m_controller;

	// Current state 
	STATE m_state;
	ENDGAME m_endgameState;
};

