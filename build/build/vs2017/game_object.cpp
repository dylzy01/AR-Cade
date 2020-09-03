#include "game_object.h"



game_object::game_object()
{
	// Basic transforms
	m_localTransform.SetIdentity();
	m_scaleTransform.SetIdentity();
	m_rotateTransform.SetIdentity();
	m_translateTransform.SetIdentity();
}

game_object::~game_object()
{
}

void game_object::Update()
{
}

// Transform //

void game_object::SetTransform(gef::Matrix44 transform)
{
	m_localTransform = transform;
}

void game_object::CombineTransforms()
{
	gef::Matrix44 rotation;
	rotation.RotationX(gef::DegToRad(m_rotateVector.x()));
	rotation.RotationY(gef::DegToRad(m_rotateVector.y()));
	rotation.RotationZ(gef::DegToRad(m_rotateVector.z()));
	
	m_localTransform = m_scaleTransform * rotation * m_translateTransform;
	set_transform(m_localTransform);
}

void game_object::UpdateTransform(gef::Matrix44 markerTransform)
{
	set_transform(m_localTransform * markerTransform);
}

// Getters //

gef::Vector4 game_object::GetVelocity()
{
	return m_velocity;
}

gef::Quaternion game_object::GetRotation()
{
	return m_rotation;
}

gef::Vector4 game_object::GetRotationVector()
{
	return m_rotateVector;
}

gef::Vector4 game_object::GetScale()
{
	return m_scaleTransform.GetScale();
}

gef::Vector4 game_object::GetPosition()
{
	return m_localTransform.GetTranslation();
}

gef::Vector4 game_object::GetDirection()
{
	return m_direction / 100;
}

gef::Vector4 game_object::GetRightVector()
{
	return transform_.GetColumn(0);
}

gef::Vector4 game_object::GetForwardVector()
{
	return m_forwardVector;
}

gef::Matrix44 game_object::GetWorldTransform()
{
	return transform_;
}

gef::Material game_object::GetMaterial()
{
	return m_material;
}

// Setters //

void game_object::SetVelocity(gef::Vector4 xyzw)
{
	m_velocity = xyzw;
	gef::Vector4 newPosition(m_localTransform.GetTranslation() + m_velocity);
	SetPosition(newPosition);
}

void game_object::SetScale(gef::Vector4 xyzw)
{
	m_scaleTransform.Scale(xyzw);
	CombineTransforms();
}

void game_object::SetRotation(gef::Quaternion xyzw)
{
	m_rotateTransform.Rotation(xyzw);
	CombineTransforms();
}

void game_object::SetRotation(gef::Vector4 xyzw)
{
	m_rotateVector = xyzw;
	CombineTransforms();
}

void game_object::SetPosition(gef::Vector4 xyzw)
{
	m_translateTransform.SetTranslation(xyzw);
	CombineTransforms();
}

void game_object::SetForwardVector(gef::Vector4 direction)
{
	m_forwardVector = direction;
}

void game_object::SetMaterial(gef::Material* material)
{
	m_material = *material;
}


