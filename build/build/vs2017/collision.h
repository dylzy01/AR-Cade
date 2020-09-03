#pragma once
#include <system/application.h>
#include <system/platform.h>
#include <vector>
#include <maths/vector2.h>
#include <maths/vector4.h>
#include <maths/matrix44.h>
#include <maths/math_utils.h>
#include <graphics/mesh_instance.h>
#include <graphics/mesh.h>



class collision
{
public:
	collision();
	~collision();

	// Collision detection
	bool CollisionSphere(gef::MeshInstance* mesh_1, gef::MeshInstance* mesh_2);
	bool CollisionAABB(gef::MeshInstance* mesh_1, gef::MeshInstance* mesh_2);

	void SATtest(const gef::Vector4& axis, const std::vector<gef::Vector4>& ptSet, float& minAlong, float& maxAlong);
};

