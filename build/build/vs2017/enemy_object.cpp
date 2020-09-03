#include "enemy_object.h"


enemy_object::enemy_object(PrimitiveBuilder* primitiveBuilder, int direction)
{
	game_object::game_object();
	
	// Define controllers
	m_direction = direction;
	m_collided = false;

	// Setup object
	set_mesh(primitiveBuilder->CreateSphereMesh(0.5f, 10.f, 10.f, gef::Vector4(0, 0, 0), &m_material));
	SetScale(gef::Vector4(0.059, 0.059, 0.059));

	// Setup radius
	m_radius = new game_object();
	m_radius->set_mesh(primitiveBuilder->CreateSphereMesh(0.5f, 10.f, 10.f, gef::Vector4(0, 0, 0), &m_material));
	m_radius->SetScale(gef::Vector4(0.35, 0.35, 0.059));
}

enemy_object::~enemy_object()
{
}

void enemy_object::Update()
{
	// Update movement
	Movement();
}

void enemy_object::Movement()
{
	// Up
	if (m_direction == 1)
	{
		SetVelocity(gef::Vector4(0, 0.001, 0));
	}
	// Down
	else if (m_direction == 2)
	{
		SetVelocity(gef::Vector4(0, -0.001, 0));
	}
	// Left
	else if (m_direction == 3)
	{
		SetVelocity(gef::Vector4(-0.001, 0, 0));
	}
	// Right
	else if (m_direction == 4)
	{
		SetVelocity(gef::Vector4(0.001, 0, 0));
	}
}

void enemy_object::Reverse()
{
	if (m_direction == 1)
	{
		m_direction = 2;
	}
	else if (m_direction == 2)
	{
		m_direction = 1;
	}
	else if (m_direction == 3)
	{
		m_direction = 4;
	}
	else if (m_direction == 4)
	{
		m_direction = 3;
	}
}

int enemy_object::GetDirection()
{
	return m_direction;
}

void enemy_object::SetDirection(int diro)
{
	m_direction = diro;
}

game_object * enemy_object::GetRadius()
{
	return m_radius;
}

bool enemy_object::GetHasCollided()
{
	return m_collided;
}

void enemy_object::SetHasCollided(bool collision)
{
	m_collided = collision;
}
