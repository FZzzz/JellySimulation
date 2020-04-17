#include "ConstraintSolver.h"

ConstraintSolver::ConstraintSolver()
	: m_solver_iteration(1)
{
}

ConstraintSolver::~ConstraintSolver()
{
	for (size_t i = 0; i < m_static_constraints.size(); ++i)
		m_static_constraints[i] = nullptr;
	for (size_t i = 0; i < m_collision_constraints.size(); ++i)
		m_collision_constraints[i] = nullptr;

	m_static_constraints.clear();
	m_collision_constraints.clear();
}

void ConstraintSolver::setSolverIteration(uint32_t iteration_num)
{
	m_solver_iteration = iteration_num;
}

bool ConstraintSolver::SolvePBDConstraints()
{
	for (Constraint* c : m_static_constraints)
	{
		c->SolveConstraint();
	}
	return true;
}

void ConstraintSolver::AddStaticConstraint(Constraint* constraint)
{
	m_static_constraints.push_back(constraint);
}

void ConstraintSolver::AddCollisionConstraint(Constraint* constraint)
{
	m_collision_constraints.push_back(constraint);
}
