#include "ConstraintSolver.h"

ConstraintSolver::ConstraintSolver()
{
}

ConstraintSolver::~ConstraintSolver()
{
}

void ConstraintSolver::setSolverIteration(uint32_t iteration_num)
{
	m_solver_iteration = iteration_num;
}

bool ConstraintSolver::SolvePBDConstraints()
{

	return true;
}
