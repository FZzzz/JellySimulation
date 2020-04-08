#include "Constraints.h"

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

DistanceConstraint::DistanceConstraint(Particle* p1, Particle* p2, float d)
	: Constraint(2)
{
}

DistanceConstraint::~DistanceConstraint()
{
}

BendConstraint::BendConstraint(Particle* p1, Particle* p2, float d)
	: Constraint(2)
{
}

BendConstraint::~BendConstraint()
{
}
