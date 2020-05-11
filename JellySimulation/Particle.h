#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include <memory>
#include <glm/common.hpp>
#include "Collider.h"

class Particle;
using Particle_Ptr = std::shared_ptr<Particle>;

struct ParticleData
{
	ParticleData(glm::vec3 pos, float m):
		position(pos), 
		velocity(glm::vec3(0)),
		force(glm::vec3(0)),
		prev_position(glm::vec3(0)),
		prev_velocity(glm::vec3(0)),
		prev_force(glm::vec3(0)),
		new_position(glm::vec3(0)),
		new_velocity(glm::vec3(0)),
		new_force(glm::vec3(0)),
		mass(m),
		massInv(1.f/mass)
	{
	}
	glm::vec3	position;
	glm::vec3	velocity;
	glm::vec3	force;

	glm::vec3	prev_position;
	glm::vec3	prev_velocity;
	glm::vec3	prev_force;

	glm::vec3   new_position;
	glm::vec3   new_velocity;
	glm::vec3	new_force;

	float		mass;
	float		massInv;
};

class Particle
{
public:
	Particle(glm::vec3 pos, float mass);
	~Particle();

	void Update(float dt);
	void UpdateCollider();

	bool TestCollision(Collider* other);
	void OnCollision(Collider* other, const float& dt);
	// Attributes
	// use pointer to maximize CPU cache performance
	ParticleData*  m_data;
	PointCollider* m_collider;
};




#endif

