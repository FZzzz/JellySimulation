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
	
	bool SolvePBDConstraints(
		std::vector<Constraint*>& static_constraints, 
		std::vector<Constraint*>& collision_constraints
	);

	/*
	TODO: Decompose these functions. Constraint sovler should not record constrains
	*/
	
	// setter
	void setSolverIteration(uint32_t iteration_num);

	//getter
	uint32_t getSolverIteration() { return m_solver_iteration; }

private:

	uint32_t m_solver_iteration;


};

#endif