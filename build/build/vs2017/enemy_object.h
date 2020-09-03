#pragma once
#include "game_object.h"

class enemy_object : public game_object
{
public:
	enemy_object(PrimitiveBuilder* primitiveBuilder, int direction);
	~enemy_object();

	// Default
	void Update();
	void Movement();
	void Reverse();

	// Getters
	int GetDirection();
	void SetDirection(int diro);
	game_object* GetRadius();
	bool GetHasCollided();

	// Setters
	void SetHasCollided(bool collision);

private:
	// Controllers
	int m_direction;
	bool m_collided;
	
	// Radius object
	game_object* m_radius;
};

