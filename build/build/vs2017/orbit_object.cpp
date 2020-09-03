#include "orbit_object.h"



orbit_object::orbit_object(PrimitiveBuilder* primitiveBuilder)
{
	// Setup object
	set_mesh(primitiveBuilder->CreateSphereMesh(0.5f, 10.f, 10.f, gef::Vector4(0, 0, 0), &m_material));
	SetScale(gef::Vector4(0.059, 0.059, 0.059));
	
	// Setup object radius
	m_radius = new game_object();
	m_radius->set_mesh(primitiveBuilder->CreateSphereMesh(0.5f, 10.f, 10.f, gef::Vector4(0, 0, 0), &m_matRadius));
	m_radius->SetScale(gef::Vector4(0.225, 0.225, 0.059));
}


orbit_object::~orbit_object()
{
}

game_object * orbit_object::GetRadius()
{
	return m_radius;
}

gef::Material * orbit_object::GetMaterialRadius()
{
	return &m_matRadius;
}

void orbit_object::SetMaterialRadius(gef::Material * mat)
{
	m_matRadius = *mat;
}


