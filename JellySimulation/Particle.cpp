#include "Particle.h"
#include "imgui/imgui.h"

Particle::Particle(glm::vec3 pos, float mass)
	:m_collider(new PointCollider(pos))
{
	m_data = new ParticleData(pos, mass);
}

Particle::~Particle()
{
	if(m_data) delete m_data;
	delete m_collider;

	m_collider = nullptr;
	m_data = nullptr;
}

void Particle::Update(float dt)
{
	m_data->prev_force = m_data->force;
	m_data->prev_velocity = m_data->velocity;
	m_data->prev_position = m_data->position;
	
	m_data->force = m_data->new_force;
	
	/* This is why it is called "Position Based Dynamics" */
	m_data->velocity = (m_data->new_position - m_data->position) / dt;
	m_data->position = m_data->new_position;

	UpdateCollider();
}

void Particle::UpdateCollider()
{
	m_collider->m_position = m_data->position;
}
