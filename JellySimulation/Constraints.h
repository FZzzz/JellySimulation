#ifndef _CONSTRAINTS_H_
#define _CONSTRAINTS_H_

#include <vector>
#include <functional>
#include "Particle.h"

enum CONSTRAINT_TYPE
{
	CONSTRAINT_DISTANCE,
	CONSTRAINT_BEND
};

class Constraint
{
public:

	std::vector<Particle*> m_particles;
	
	Constraint(size_t numOfRigidbodies);
	~Constraint();

	virtual bool SolveConstraint() = 0;
	
	virtual float ConstraintFunction() = 0;
	virtual glm::vec3 GradientFunction() = 0;
	
	// getter
	virtual CONSTRAINT_TYPE getConstraintType() = 0;

};

class DistanceConstraint: public Constraint
{
public:
	
	// Not allow to use C++ default constructor
	DistanceConstraint() = delete;
	/*
	 * @param p1 First particle
	 * @param p2 Second paritcle
	 * @param d  Initial distance of two particle
	 */
	DistanceConstraint(Particle* p1, Particle* p2, float d);
	~DistanceConstraint();

	CONSTRAINT_TYPE getConstraintType() { return CONSTRAINT_TYPE::CONSTRAINT_DISTANCE; };

};


class BendConstraint : public Constraint
{
public:
	
	// Not allow to use C++ default constructor
	BendConstraint() = delete;
	/*
	 * @param p1 First particle
	 * @param p2 Second paritcle
	 * @param d  Initial distance of two particle
	 */
	BendConstraint(Particle* p1, Particle* p2, float d);
	~BendConstraint();

	CONSTRAINT_TYPE getConstraintType() { return CONSTRAINT_TYPE::CONSTRAINT_BEND; };

};




#endif
