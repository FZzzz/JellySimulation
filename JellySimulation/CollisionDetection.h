#ifndef _COLLISION_DETECTION_H_
#define _COLLISION_DETECTION_H_

#include "Collider.h"

// forward declaration
class Collider;
class PointCollider;
class SphereCollider;
class AABB;
class OBB;
class Plane;

namespace CollisionDetection
{
	bool PointSphereIntersection(PointCollider* point, SphereCollider* sphere);
	bool PointAABBIntersection(PointCollider* point, AABB* aabb);
	bool PointOBBIntersection(PointCollider* point, OBB* obb);
	bool PointPlaneIntersection(PointCollider* point, Plane* plane);
	bool PointPlaneIntersection(glm::vec3 p, Plane* plane);
	
	bool SphereSphereIntersection(SphereCollider* sphere0, SphereCollider* sphere1);
	bool SphereAABBIntersection(SphereCollider* sphere, AABB* aabb);
	bool SphereOBBIntersection(SphereCollider* sphere, OBB* obb);
	bool SpherePlaneIntersection(SphereCollider* sphere, Plane* plane);
	
	bool AABBAABBIntersection(AABB* box0, AABB* box1);
	bool AABBOBBIntersection(AABB* aabb, OBB* obb);
	bool AABBPlaneIntersection(AABB* aabb, Plane* plane);
	
	bool OBBOBBIntersection(OBB* box0, OBB* box1);
};

#endif