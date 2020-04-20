#include "Simulation.h"
#include "imgui/imgui.h"
#include <iostream>

Simulation::Simulation()
	: m_solver(nullptr), m_initialized(false), m_world_desc(SimWorldDesc(-9.8f, 0.f))
{
	

}
Simulation::Simulation(SimWorldDesc desc)
	: m_solver(nullptr), m_initialized(false), m_world_desc(desc)
{
}

Simulation::~Simulation()
{
}

void Simulation::Initialize()
{
	m_solver = std::make_shared<ConstraintSolver>();
	m_initialized = true;
}

bool Simulation::Step(float dt)
{
	if (!m_initialized)
		Initialize();

	UpdatePhysics(dt);
	CollisionDetection();
	GenerateCollisionConstraint();
	
 	if (!ProjectConstraints())
		return false;

	ApplySolverResults(dt);

	return true;
}

void Simulation::AddParticle(Particle* particle)
{
	m_particles.push_back(particle);
}

void Simulation::AddCollider(Collider* collider)
{
	m_colliders.push_back(collider);
	m_collision_table.push_back(std::vector<Collider*>());
}

void Simulation::AddStaticConstraint(Constraint* constraint)
{
	m_static_constraints.push_back(constraint);
}

void Simulation::SetSolverIteration(uint32_t iter_count)
{
	m_solver->setSolverIteration(iter_count);
}

void Simulation::setGravity(float gravity)
{
	m_world_desc.gravity = gravity;
}

void Simulation::UpdatePhysics(float dt)
{
	/*
	 * Update position and velocity
	 * 1. forall vertices i do v_i = v_i + dt * w_i * f_{ext}
	 * 2. damp velocities 	
	 */
	for (auto p : m_particles)
	{
		// external forces
		p->m_data->force = glm::vec3(0, m_world_desc.gravity, 0);

		p->m_data->velocity = p->m_data->velocity + dt * p->m_data->massInv * p->m_data->force;
		
		// dampVelocity()

		p->m_data->new_position = p->m_data->position + dt * p->m_data->velocity;

		// Update colliders
		p->UpdateCollider();
	}

	for (auto c : m_colliders)
	{
		
	}


}

void Simulation::CollisionDetection()
{	
	// Clean previous CD result
	for (auto vec : m_collision_table)
	{
		vec.clear();
	}

	// TODO: Change to m_rigidbodies[i], m_rigidbodies[j]
	for (size_t i = 0; i < m_colliders.size(); ++i)
	{
		for (size_t j = i+1; j < m_colliders.size(); ++j)
		{
			/* Record result if there's contact between two objects */
			if (m_colliders[i]->TestCollision(m_colliders[j]))
				m_collision_table[i].push_back(m_colliders[j]);
		}
	}
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
	m_solver->SolvePBDConstraints(m_static_constraints, m_collision_constraints);

	return true;
}

void Simulation::AddCollisionConstraint(Constraint* constraint)
{
	m_collision_constraints.push_back(constraint);
}

void Simulation::ApplySolverResults(float dt)
{
	for(auto p : m_particles)
	{
		p->Update(dt);
	}
	/*
	std::cout << m_particles[0]->m_data->position.x << " "
		<< m_particles[1]->m_data->position.x << std::endl;
	*/
/*
#ifdef _DEBUG
	{
		ImGui::Begin("Particles");
		float p0[3] = { m_particles[0]->m_data->position.x,
						m_particles[0]->m_data->position.y,
						m_particles[0]->m_data->position.z };
		ImGui::InputFloat3("P0 position", p0, 5, ImGuiInputTextFlags_ReadOnly);

		float p1[3] = { m_particles[1]->m_data->position.x,
						m_particles[1]->m_data->position.y,
						m_particles[1]->m_data->position.z };
		ImGui::InputFloat3("P1 position", p1, 5, ImGuiInputTextFlags_ReadOnly);
		ImGui::End();
	}
#endif
*/

}
