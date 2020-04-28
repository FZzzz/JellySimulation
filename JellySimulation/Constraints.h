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

	
	Constraint() = delete;
	Constraint(size_t numOfRigidbodies);
	~Constraint();

	virtual bool SolveConstraint() = 0;
	
	virtual float ConstraintFunction() = 0;
	virtual std::vector<std::vector<float>> GradientFunction() = 0;
	
	// getter
	virtual CONSTRAINT_TYPE getConstraintType() = 0;

	/* 
	 * Constraint caches particles but not owning it
	 * TODO: Rewrite this (Not generic solution) 
	 */
	std::vector<Particle*> m_particles;


};

class DistanceConstraint final: public Constraint
{
public:
	
	// Not allow to use C++ default constructor
	DistanceConstraint() = delete;
	/*
	 * @param p1 First particle
	 * @param p2 Second paritcle
	 * @param d  Initial distance of two particle
	 */
	DistanceConstraint(Particle* p0, Particle* p1, float d);
	~DistanceConstraint();

	virtual bool SolveConstraint();

	virtual float ConstraintFunction();
	virtual std::vector<std::vector<float>> GradientFunction();

	// setters
	void setStiffness(float stiffness);

	// getters
	CONSTRAINT_TYPE getConstraintType() { return CONSTRAINT_TYPE::CONSTRAINT_DISTANCE; };

private: 

	// stiffness is the value between 0-1
	float m_stiffness;
	float m_rest_length;
};


class BendConstraint final: public Constraint
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

	virtual bool SolveConstraint();

	virtual float ConstraintFunction();
	virtual std::vector<std::vector<float>> GradientFunction();

	CONSTRAINT_TYPE getConstraintType() { return CONSTRAINT_TYPE::CONSTRAINT_BEND; };

};




#endif
