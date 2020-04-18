#include "Collider.h"
#include "CollisionDetection.h"


Collider::Collider(ColliderTypes type)
	: m_type(type)
{
}

Collider::~Collider()
{
}

PointCollider::PointCollider(glm::vec3 pos)
	: Collider(ColliderTypes::POINT), m_position(pos)
{
}

PointCollider::~PointCollider()
{
}

void PointCollider::UpdateCollider()
{
}

bool PointCollider::TestCollision(Collider* other)
{
	bool result = false;

	switch (other->getColliderTypes())
	{
	case ColliderTypes::SPHERE:
		result = CollisionDetection::PointSphereIntersection(this, dynamic_cast<SphereCollider*>(other));
		break;
	case ColliderTypes::AABB:
		result = CollisionDetection::PointAABBIntersection(this, dynamic_cast<AABB*>(other));
		break;
	case ColliderTypes::OBB:
		result = CollisionDetection::PointOBBIntersection(this, dynamic_cast<OBB*>(other));
		break;
	case ColliderTypes::PLANE:
		result = CollisionDetection::PointSphereIntersection(this, dynamic_cast<SphereCollider*>(other));
		break;
	default:
		break;
	}
	
	return result;
}

SphereCollider::SphereCollider(glm::vec3 center, float radius)
	: Collider(ColliderTypes::SPHERE), m_center(center), m_radius(radius)
{
}

SphereCollider::~SphereCollider()
{
}

void SphereCollider::UpdateCollider()
{
}

bool SphereCollider::TestCollision(Collider* collider)
{
	bool result = false;
	return result;
}

AABB::AABB(glm::vec3 min, glm::vec3 max)
	: Collider(ColliderTypes::AABB), m_min(min), m_max(max)
{
}

AABB::~AABB()
{
}

void AABB::UpdateCollider()
{
}

bool AABB::TestCollision(Collider* other)
{
	bool result = false;
	return result;
}

OBB::OBB(glm::vec3 center, glm::vec3 local_x_axis, glm::vec3 local_y_axis, glm::vec3 local_z_axis, float extend)
	: Collider(ColliderTypes::OBB),
	m_local_axis{local_x_axis, local_y_axis, local_z_axis},
	m_extend(extend)
{
}

OBB::~OBB()
{
}

void OBB::UpdateCollider()
{
}

bool OBB::TestCollision(Collider* other)
{
	bool result = false;
	return result;
}

Plane::Plane(glm::vec3 plane_normal, float d)
	: Collider(ColliderTypes::PLANE), m_normal(plane_normal), m_d(d)
{
}

Plane::~Plane()
{
}
