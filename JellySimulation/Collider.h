#ifndef _COLLIDER_H_
#define _COLLIDER_H_

#include <glm/glm.hpp>

class Collider
{
public:
	enum class ColliderTypes
	{
		POINT = 0,
		SPHERE,
		AABB,
		OBB,
		PLANE
	};

	Collider() = delete;
	Collider(ColliderTypes type);
	~Collider();

	virtual void UpdateCollider() = 0;
	virtual bool TestCollision(Collider* other) = 0;

	// getters
	__forceinline ColliderTypes getColliderTypes() { return m_type; };

private:
	ColliderTypes m_type;

};

/* 
 * Performance issue: Extra allocation needed
 */
class PointCollider final : public Collider
{
public:
	PointCollider(glm::vec3 pos);
	~PointCollider();

	virtual void UpdateCollider();
	virtual bool TestCollision(Collider* other);
	
	glm::vec3 m_position;
};

class SphereCollider final : public Collider
{
public:
	SphereCollider() = delete;
	SphereCollider(glm::vec3 center, float radius);
	~SphereCollider();

	virtual void UpdateCollider();
	virtual bool TestCollision(Collider* collider);

	glm::vec3 m_center;
	float	  m_radius;

};

class AABB final : public Collider
{
public:
	AABB() = delete;
	AABB(glm::vec3 min, glm::vec3 max);
	~AABB();

	virtual void UpdateCollider();
	virtual bool TestCollision(Collider* other);

	glm::vec3 m_min;
	glm::vec3 m_max;

};

class OBB final : public Collider
{
public:
	OBB() = delete;
	OBB(glm::vec3 center,
		glm::vec3 local_x_axis,
		glm::vec3 local_y_axis,
		glm::vec3 local_z_axis,
		float extend);
	~OBB();

	virtual void UpdateCollider();
	virtual bool TestCollision(Collider* other);

	glm::vec3 m_center;
	glm::vec3 m_local_axis[3];
	float m_extend;

};

class Plane final : public Collider
{
public:
	Plane() = delete;
	Plane(glm::vec3 plane_normal, float distance_from_orgin);
	~Plane();

	glm::vec3 m_normal;
	float m_d;

};


#endif 
