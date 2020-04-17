#ifndef _CONSTRAINT_SOLVER_H_
#define _CONSTRAINT_SOLVER_H_

#include <vector>
#include <cstdint>
#include "Constraints.h"


class ConstraintSolver
{
public:
	ConstraintSolver();
	~ConstraintSolver();
	
	bool SolvePBDConstraints();

	/*
	TODO: Decompose these functions. Constraint sovler should not record constrains
	*/
	void AddStaticConstraint(Constraint* constraint);
	void AddCollisionConstraint(Constraint* constraint);

	// setter
	void setSolverIteration(uint32_t iteration_num);

	//getter
	uint32_t getSolverIteration() { return m_solver_iteration; }

private:

	uint32_t m_solver_iteration;
	std::vector<Constraint*> m_static_constraints;
	std::vector<Constraint*> m_collision_constraints;


};

#endif