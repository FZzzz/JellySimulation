#include "Particle.h"

Particle::Particle(glm::vec3 pos, float mass)
{
	m_data = new ParticleData(pos, mass);
}

Particle::~Particle()
{
	if(m_data)
		delete m_data;
	m_data = nullptr;
}
