#pragma once
#include "game_object.h"

class player_object : public game_object
{
public:
	player_object(PrimitiveBuilder* primitiveBuilder);
	~player_object();

	// Default
	void Update(float deltaTime);
	
	// Player movement/orbit
	void Move();
	gef::Vector4 CreateRotation(const gef::Vector4 forward, const gef::Vector4 up, double theta);
	
	// Player lives
	void DecrementLives();
	void StartImmunityTimer();

	// Getters
	bool GetLockOn();
	float GetTurn();
	float GetTotalTurn();
	bool GetShouldOrbit();
	int GetTargetOrbit();
	bool GetCanDie();
	int GetLives();
	
	// Setters
	void SetLockOn(bool lockOn);
	void SetTurn(float turn);
	void SetTotalTurn(float totalTurn);
	bool SetShouldOrbit(bool rotate);
	void SetTargetOrbit(int target);
	void SetCanDie(bool canDie);

private:
	// Orbit
	bool m_lockOn = false;
	float m_turn = 0.f;
	float m_totalTurn = 0.f;
	bool m_shouldOrbit = false;
	int m_targetOrbit = -1;

	// Timer
	float m_elapsedTime = 0;
	float m_deltaTime = 0;

	// Lives
	bool m_canDie;
	int m_lives;
};

