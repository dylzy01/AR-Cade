#include "player_object.h"

player_object::player_object(PrimitiveBuilder* primitiveBuilder)
{
	game_object::game_object();
	
	// Default turn direction
	m_turn = 75.f;
	m_totalTurn = 0;
	
	// Lives
	m_canDie = true;
	m_lives = 3;
	
	// Setup mesh
	set_mesh(primitiveBuilder->CreateSphereMesh(0.5f, 10.f, 10.f, gef::Vector4(0, 0, 0), &m_material));
	SetScale(gef::Vector4(0.05, 0.05, 0.059));
	SetRotation(gef::Vector4(0.f, 0.f, 0.f));
}

player_object::~player_object()
{
}

void player_object::Update(float deltaTime)
{
	// Define delta time
	m_deltaTime = deltaTime;

	// Movement
	Move();
	
	// Player immunity
	if (!m_canDie)
	{
		StartImmunityTimer();
	}
}

void player_object::Move()
{
	// Localised turn amount
	float turn = m_turn;

	// Should the player rotate around the target orbit ?
	if (m_shouldOrbit)
	{
		// Accumulate turn amount & total turn amount
		turn *= m_deltaTime;
		m_totalTurn += turn;

		// Set new rotation, based on accumulated turn amount
		SetRotation(gef::Vector4(GetRotationVector().x(), GetRotationVector().y(), GetRotationVector().z() + turn));
	}

	// Rotate 'forward' (right) vector
	SetForwardVector(CreateRotation(GetRightVector(), gef::Vector4(0.0f, 0.0f, 1.0f), gef::DegToRad(m_totalTurn)));

	// Move along 'forward' (right) vector
	gef::Vector4 pos = GetPosition() + gef::Vector4(GetRightVector().x() * (2.5f * m_deltaTime), GetRightVector().y() * (2.5f * m_deltaTime), 0.0f);
	SetPosition(pos);
}

gef::Vector4 player_object::CreateRotation(const gef::Vector4 forward, const gef::Vector4 up, double theta)
{
	double thetaCOS = cos(theta);
	double thetaSIN = sin(theta);

	gef::Vector4 rotator = (forward * thetaCOS) + (up.CrossProduct(forward) * thetaSIN) + (up * up.DotProduct(forward)) * (1 - thetaCOS);

	return rotator;
}

void player_object::DecrementLives()
{
	m_lives--;
}

void player_object::StartImmunityTimer()
{
	m_elapsedTime += m_deltaTime;
	if (m_elapsedTime >= 3)
	{
		m_elapsedTime = 0;
		m_canDie = true;
	}
}

bool player_object::GetLockOn()
{
	return m_lockOn;
}

float player_object::GetTurn()
{
	return m_turn;
}

float player_object::GetTotalTurn()
{
	return m_totalTurn;
}

bool player_object::GetShouldOrbit()
{
	return m_shouldOrbit;
}

int player_object::GetTargetOrbit()
{
	return m_targetOrbit;
}

bool player_object::GetCanDie()
{
	return m_canDie;
}

int player_object::GetLives()
{
	return m_lives;
}

void player_object::SetLockOn(bool lockOn)
{
	m_lockOn = lockOn;
}

void player_object::SetTurn(float turn)
{
	m_turn = turn;
}

void player_object::SetTotalTurn(float totalTurn)
{
	m_totalTurn = totalTurn;
}

bool player_object::SetShouldOrbit(bool shouldOrbit)
{
	return m_shouldOrbit = shouldOrbit;
}

void player_object::SetTargetOrbit(int target)
{
	m_targetOrbit = target;
}

void player_object::SetCanDie(bool canDie)
{
	m_canDie = canDie;

	/*if (!canDie)
	{
		StartImmunityTimer();
	}*/
}
