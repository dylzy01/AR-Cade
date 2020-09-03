#pragma once
#include "game_object.h"

class orbit_object : public game_object
{
public:
	orbit_object(PrimitiveBuilder* primitiveBuilder);
	~orbit_object();

	// Getters
	game_object* GetRadius();
	gef::Material* GetMaterialRadius();

	// Setters
	void SetMaterialRadius(gef::Material* mat);
		
private:
	// Radius object
	game_object* m_radius;
	gef::Material m_matRadius;
};

