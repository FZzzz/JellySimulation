#ifndef _SIMULATION_H_
#define _SIMULATION_H_

#include <stdint.h>
#include <memory>
#include "Constraints.h"
#include "ConstraintSolver.h"


class Simulation
{

public:
	Simulation();
	~Simulation();

	void Initialize();
	bool Step(float dt);

private:

	void UpdatePhysics(float dt);
	void CollisionDetection();
	void GenerateCollisionConstraint();
	bool ProjectConstraints();

	std::shared_ptr<ConstraintSolver> m_solver;

};

#endif
