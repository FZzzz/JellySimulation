#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include <memory>
#include <glm/common.hpp>

class Particle;
using Particle_Ptr = std::shared_ptr<Particle>;

struct ParticleData
{
	ParticleData(glm::vec3 pos, float m):
		position(pos), 
		velocity(glm::vec3(0)),
		force(glm::vec3(0)),
		mass(m),
		massInv(1.f/mass)
	{
	}
	glm::vec3	position;
	glm::vec3	velocity;
	glm::vec3	force;
	float		mass;
	float		massInv;
};

class Particle
{
public:
	Particle(glm::vec3 pos, float mass);
	~Particle();
	// attributes
	// use pointer to maximize CPU cache performance
	ParticleData* m_data;
};




#endif

