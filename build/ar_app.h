#ifndef _RENDER_TARGET_APP_H
#define _RENDER_TARGET_APP_H

#include <system/application.h>
#include <system/platform.h>
#include <maths/vector2.h>
#include <maths/math_utils.h>
#include <vector>
#include <platform/vita/graphics/texture_vita.h>
#include <graphics/sprite.h>
#include <graphics/sprite_renderer.h>
#include <graphics/texture.h>
#include <graphics/mesh.h>
#include <graphics/mesh_instance.h>
#include <graphics/primitive.h>
#include <graphics/renderer_3d.h>
#include <graphics/render_target.h>
#include <graphics/image_data.h>
#include <graphics/font.h>
#include <input/touch_input_manager.h>
#include <input/input_manager.h>
#include <assets/png_loader.h>
#include <sony_sample_framework.h>
#include <sony_tracking.h>
#include <gef_abertay/input/sony_controller_input_manager.h>
#include <cstdlib>
#include <ctime>
#include <random>
#include <algorithm>

// Vita AR includes
#include <camera.h>
#include <gxm.h>
#include <motion.h>
#include <libdbg.h>
#include <libsmart.h>
#include <graphics/colour.h>

// Header includes
#include "primitive_builder.h"
#include "collision.h"
#include "game_object.h"
#include "orbit_object.h"
#include "enemy_object.h"
#include "player_object.h"
#include "load_texture.h"
#include "states.h"
#include "splash_state.h"
#include "howto_state.h"
#include "game_state.h"
#include "endgame_state.h"

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class Renderer3D;
	class Mesh;
	class RenderTarget;
	class TextureVita;
	class InputManager;
	class Scene;
}

class ARApp : public gef::Application
{
public:
	ARApp(gef::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float deltaTime);
	void Render();
private:
	void SetupLights();
	void HandleInput();
	void UpdateCurrentState();
	void SetCurrentState(STATE state);
	
private:
	// States
	STATE m_currentState;
	splash_state* m_splashState;
	howto_state* m_howtoState;
	game_state* m_gameState;
	endgame_state* m_endGameState;

	// GEF
	gef::InputManager* m_inputManager;
	gef::SpriteRenderer* m_spriteRenderer;
	gef::Font* m_font;
	class gef::Renderer3D* m_renderer3d;
	PrimitiveBuilder* m_primitiveBuilder;

	// Frames per second
	float m_fps;

	// ar_app controllers
	bool m_initialiseOnce, m_cleanUpOnce;
};




#endif // _RENDER_TARGET_APP_H