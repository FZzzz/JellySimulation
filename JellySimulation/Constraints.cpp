#include <glm/glm.hpp>
#include <assert.h>
#include "Constraints.h"

#define EPSILON 0.0000000001f 

Constraint::Constraint(size_t numOfRigidbodies)
{
	m_particles.resize(numOfRigidbodies, nullptr);
}

Constraint::~Constraint()
{
	for (size_t i = 0; i < m_particles.size(); ++i)
		m_particles[i] = nullptr;
	m_particles.clear();
}

BendConstraint::BendConstraint(Particle* p1, Particle* p2, float d)
	: Constraint(2)
{

}

BendConstraint::~BendConstraint()
{
}

DistanceConstraint::DistanceConstraint(Particle* p0, Particle* p1, float d)
	: Constraint(2), m_rest_length(d), m_stiffness(1.f)
{
	m_particles[0] = p0;
	m_particles[1] = p1;
}

DistanceConstraint::~DistanceConstraint()
{
}

bool DistanceConstraint::SolveConstraint()
{
	glm::vec3 correction[2];

	ParticleData& p0_data = *(m_particles[0]->m_data);
	ParticleData& p1_data = *(m_particles[1]->m_data);

	const float& w0 = p0_data.massInv;
	const float& w1 = p1_data.massInv;

	float w_sum = w0 + w1;
	float distance = glm::distance(p0_data.new_position, p1_data.new_position);
	float C = distance - m_rest_length;
	glm::vec3 v = p0_data.new_position - p1_data.new_position;

	//assert(distance < EPSILON);
	if (distance < EPSILON)
		distance = EPSILON;
	
	glm::vec3 n = v / distance;
	
	correction[0] = (-w0 / w_sum) * C * n;
	correction[1] = (w1 / w_sum) * C * n;

	// Correction 
	p0_data.new_position += m_stiffness * correction[0];
	p1_data.new_position += m_stiffness * correction[1];

	return true;
}

float DistanceConstraint::ConstraintFunction()
{
	float constraint_value = 0.f;
	const glm::vec3& p0 = m_particles[0]->m_data->position;
	const glm::vec3& p1 = m_particles[1]->m_data->position;

	constraint_value = glm::distance(p0, p1) - m_rest_length;	

	return constraint_value;
}

std::vector<std::vector<float>> DistanceConstraint::GradientFunction()
{
	std::vector<std::vector<float>> jacobian;

	jacobian.resize(1, std::vector<float>(3, 0));
	
	const glm::vec3& p0 = m_particles[0]->m_data->position;
	const glm::vec3& p1 = m_particles[1]->m_data->position;

	glm::vec3 n = p0 - p1;
	jacobian[0][0] = n.x;
	jacobian[0][1] = n.y;
	jacobian[0][2] = n.z;


	return jacobian;
}

void DistanceConstraint::setStiffness(float stiffness)
{
	m_stiffness = stiffness;
}

bool BendConstraint::SolveConstraint()
{
	return true;
}

float BendConstraint::ConstraintFunction()
{
	return 0.0f;
}

std::vector<std::vector<float>> BendConstraint::GradientFunction()
{
	return std::vector<std::vector<float>>();
}

