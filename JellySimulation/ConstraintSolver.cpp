#include "ConstraintSolver.h"

ConstraintSolver::ConstraintSolver()
	: m_solver_iteration(1)
{
}

ConstraintSolver::~ConstraintSolver()
{
}

void ConstraintSolver::setSolverIteration(uint32_t iteration_num)
{
	m_solver_iteration = iteration_num;
}

bool ConstraintSolver::SolvePBDConstraints(
	std::vector<Constraint*>& static_constraints,
	std::vector<Constraint*>& collision_constraints)
{
	for (uint32_t i = 0; i < m_solver_iteration; ++i)
	{
		for (Constraint* c : static_constraints)
		{
			c->SolveConstraint();
		}
	}
	
	return true;
}
