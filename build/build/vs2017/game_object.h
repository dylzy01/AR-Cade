#pragma once
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>
#include <maths/quaternion.h>
#include <vector>
#include <maths/math_utils.h>

class game_object : public gef::MeshInstance
{
public:
	game_object();
	~game_object();

	virtual void Update();

	// Transform //
	void SetTransform(gef::Matrix44 transform);
	void CombineTransforms();
	void UpdateTransform(gef::Matrix44 markerTransform);

	// Getters //
	gef::Vector4 GetVelocity();
	gef::Quaternion GetRotation();
	gef::Vector4 GetRotationVector();
	gef::Vector4 GetScale();
	gef::Vector4 GetPosition();
	gef::Vector4 GetDirection();
	gef::Vector4 GetRightVector();
	gef::Vector4 GetForwardVector();
	gef::Matrix44 GetWorldTransform();
	gef::Material GetMaterial();

	// Setters //
	void SetVelocity(gef::Vector4 xyzw);
	void SetRotation(gef::Quaternion xyzw);
	void SetRotation(gef::Vector4 xyzw);
	void SetScale(gef::Vector4 xyzw);
	void SetPosition(gef::Vector4 xyzw);
	void SetForwardVector(gef::Vector4 direction);
	void SetMaterial(gef::Material* material);

protected:
	// Transform //
	gef::Matrix44 m_localTransform, m_scaleTransform, m_rotateTransform, m_translateTransform;
	gef::Vector4 m_rotateVector;

	// Velocity
	gef::Vector4 m_velocity;

	// Rotation quaternion
	gef::Quaternion m_rotation;

	// Translation
	gef::Vector4 m_position;

	// Forward vector
	gef::Vector4 m_direction{ 1, 0, 0 };
	gef::Vector4 m_forwardVector = m_direction / 1000;

	// Material
	gef::Material m_material;
};

