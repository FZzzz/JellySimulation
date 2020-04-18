#include "CollisionDetection.h"

bool CollisionDetection::PointSphereIntersection(PointCollider* point, SphereCollider* sphere)
{
	return false;
}

bool CollisionDetection::PointAABBIntersection(PointCollider* point, AABB* aabb)
{
	const glm::vec3& point_pos = point->m_position;

	return (point_pos.x < aabb->m_max.x && point_pos.x > aabb->m_min.x &&
			point_pos.y < aabb->m_max.y && point_pos.y > aabb->m_min.y &&
			point_pos.z < aabb->m_max.z && point_pos.z > aabb->m_min.z);
}

bool CollisionDetection::PointOBBIntersection(PointCollider* point, OBB* obb)
{
	return false;
}

bool CollisionDetection::PointPlaneIntersection(PointCollider* point, Plane* plane)
{
	return false;
}

bool CollisionDetection::SphereSphereIntersection(SphereCollider* sphere0, SphereCollider* sphere1)
{
	return false;
}

bool CollisionDetection::SphereAABBIntersection(SphereCollider* sphere, AABB* aabb)
{
	return false;
}

bool CollisionDetection::SphereOBBIntersection(SphereCollider* sphere, OBB* obb)
{
	return false;
}

bool CollisionDetection::SpherePlaneIntersection(SphereCollider* sphere, Plane* plane)
{
	return false;
}

bool CollisionDetection::AABBOBBIntersection(AABB* aabb, OBB* obb)
{
	return false;
}

bool CollisionDetection::AABBAABBIntersection(AABB* box0, AABB* box1)
{
	return false;
}

bool CollisionDetection::AABBPlaneIntersection(AABB* aabb, Plane* plane)
{
	return false;
}

bool CollisionDetection::OBBOBBIntersection(OBB* box0, OBB* box1)
{
	return false;
}
