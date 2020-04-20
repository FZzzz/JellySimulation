#ifndef _SIMULATION_H_
#define _SIMULATION_H_

#include <stdint.h>
#include <memory>
#include <vector>
#include "Constraints.h"
#include "ConstraintSolver.h"
#include "Collider.h"
#include "Rigidbody.h"

struct SimWorldDesc
{
	SimWorldDesc(float g, float damping) : gravity(g), global_velocity_damping(damping) {};
	
	float gravity;
	float global_velocity_damping;
};


class Simulation
{

public:
	Simulation();
	Simulation(SimWorldDesc desc);
	~Simulation();

	void Initialize();
	bool Step(float dt);
	void AddParticle(Particle* particle);
	void AddCollider(Collider* collider);
	void AddStaticConstraint(Constraint* constraint);
	void SetSolverIteration(uint32_t iter_count);


	// setters
	void setGravity(float gravity);

	// getters
	inline float getGravity() { return m_world_desc.gravity; };
private:

	void UpdatePhysics(float dt);
	void CollisionDetection();
	void GenerateCollisionConstraint();
	bool ProjectConstraints();
	
	void AddCollisionConstraint(Constraint* constraint);
	void ApplySolverResults(float dt);

	SimWorldDesc m_world_desc;
	bool m_initialized;

	std::shared_ptr<ConstraintSolver> m_solver;

	std::vector<Particle*> m_particles;
	std::vector<Collider*> m_colliders;
	std::vector<Rigidbody*> m_rigidbodies;
	std::vector<Constraint*> m_static_constraints;
	std::vector<Constraint*> m_collision_constraints;

	/* The collision table record who contact with who */
	std::vector<std::vector<Collider*>> m_collision_table;
	
};

#endif
