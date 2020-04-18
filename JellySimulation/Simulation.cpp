#include "Simulation.h"

Simulation::Simulation()
	: m_solver(nullptr)
{
}

Simulation::~Simulation()
{
}

void Simulation::Initialize()
{
	m_solver = std::make_shared<ConstraintSolver>();
}

bool Simulation::Step(float dt)
{
	UpdatePhysics(dt);
	CollisionDetection();
	GenerateCollisionConstraint();
	
	if (!ProjectConstraints())
		return false;

	return true;
}

void Simulation::UpdatePhysics(float dt)
{
}

void Simulation::CollisionDetection()
{
	/*
	for(size_t i = 0; i < m_colliders.size();++i)
		for(size_t j = 0; j < m_colliders.size(); ++j)
		{
			 if(i==j) continue;
			 bool contact = m_colliders[i]->TestCollision(, m_colliders[j]);
			 if(contact) 
				GenerateCollisionPair();
		}

	*/
}

/*
 * In jelly simulation the only collision is particle hitting the plane or other static BBs.
 * 
*/
void Simulation::GenerateCollisionConstraint()
{
	/*
	for(pairs : collision_pairs)
	{
		// Generate collision constraint
	}
	*/


}

bool Simulation::ProjectConstraints()
{
	m_solver->SolvePBDConstraints();
	
	return true;
}
