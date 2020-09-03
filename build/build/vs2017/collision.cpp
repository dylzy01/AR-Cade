#include "collision.h"



collision::collision()
{
}

collision::~collision()
{
}

bool collision::CollisionSphere(gef::MeshInstance * mesh_1, gef::MeshInstance * mesh_2)
{
	// Create sphere objects for collision
	gef::Sphere sphere_1, sphere_2;

	// Set transform bounding spheres
	sphere_1 = mesh_1->mesh()->bounding_sphere().Transform(mesh_1->transform());
	sphere_2 = mesh_2->mesh()->bounding_sphere().Transform(mesh_2->transform());

	// Override bounding sphere radii
	gef::Sphere mesh_1_bounds(mesh_1->transform().GetTranslation(), (sphere_1.radius()));
	gef::Sphere mesh_2_bounds(mesh_2->transform().GetTranslation(), (sphere_2.radius()));

	// Apply radii to spheres	
	sphere_1.set_radius(mesh_1_bounds.radius());
	sphere_2.set_radius(mesh_2_bounds.radius());

	// Calculate the squared distance between the centres of both spheres
	gef::Vector4 vectorDistance(sphere_2.position() - sphere_1.position());
	float distanceSquared = vectorDistance.DotProduct(vectorDistance);

	// Calculate the squared sum of both radii
	float radiiSumSquared = (sphere_1.radius() + sphere_2.radius());
	radiiSumSquared *= radiiSumSquared;

	// Check for collision
	// If the distance squared is <= the square sum of the radii, we have a collision
	if (distanceSquared <= radiiSumSquared)
	{
		return true;
	}

	// If not, we don't have a collision
	return false;
}

bool collision::CollisionAABB(gef::MeshInstance * mesh_1, gef::MeshInstance * mesh_2)
{
	gef::Aabb boxOne, boxTwo;

	// Box One
	boxOne = mesh_1->mesh()->aabb();
	boxOne = boxOne.Transform(mesh_1->transform());

	// Box Two
	boxTwo = mesh_2->mesh()->aabb();
	boxTwo = boxTwo.Transform(mesh_2->transform());

	// Check if (box_1 max > box_2 min) && (box_1 min < box_2 max)
	return (
		boxOne.max_vtx().x() > boxTwo.min_vtx().x() &&
		boxOne.min_vtx().x() < boxTwo.max_vtx().x() &&
		boxOne.max_vtx().y() > boxTwo.min_vtx().y() &&
		boxOne.min_vtx().y() < boxTwo.max_vtx().y() &&
		boxOne.max_vtx().z() > boxTwo.min_vtx().z() &&
		boxOne.min_vtx().z() < boxTwo.max_vtx().z()
		);
}

void collision::SATtest(const gef::Vector4 & axis, const std::vector<gef::Vector4>& ptSet, float & minAlong, float & maxAlong)
{
	minAlong = HUGE_VAL, maxAlong = -HUGE_VAL;
	for (int i = 0; i < ptSet.size(); i++)
	{
		// just dot it to get the min/max along this axis
		float dotVal = ptSet[i].DotProduct(axis);
		if (dotVal < minAlong)
		{
			minAlong = dotVal;
		}
		if (dotVal > maxAlong)
		{
			maxAlong = dotVal;
		}
	}
}
