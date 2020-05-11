#ifndef _JELLY_H_
#define _JELLY_H_

#include <vector>
#include "Mesh.h"
#include "Particle.h"
#include "Constraints.h"


// forward declaration
struct Spring;
class Jelly;

using Jelly_Ptr = std::shared_ptr<Jelly>;


/**
 * A jelly is represented by mass-spring system.
 */
class Jelly
{
public:
	Jelly();
	~Jelly();

	void Update();

	void Initialize(std::vector<std::vector<std::vector<Particle_Ptr>>>& particles, std::vector<unsigned int> indices);

	// setter
	void setParticles(std::vector<std::vector<std::vector<Particle_Ptr>>>&	particles);
	//void setSprings(std::vector<)

	//getters
	std::shared_ptr<Mesh> getJellyMesh() { return m_jelly_mesh; };

private:

	// Refresh GPU vertex buffer data (positions) 
	void UpdateVBO();

	/*Particles*/
	void UpdateParticlePositions();

	/*Basic properties*/
	std::vector<glm::vec3>		m_positions;


	std::vector<std::vector<std::vector<Particle_Ptr>>>	m_particles;
	size_t						m_num_of_particles;
	// Buffer in Jelly class for faster update
	// TODO: Change to SOA (Structure of Array)
	// Or find a way using pointer to manage these

	/*Springs*/
	std::vector<DistanceConstraint*>		m_shear_springs;
	std::vector<DistanceConstraint*>		m_structural_springs;
	std::vector<DistanceConstraint*>		m_bend_springs;

	/*Mesh of jelly*/
	std::shared_ptr<Mesh> m_jelly_mesh;

	/*Mesh of Particles*/
	std::shared_ptr<Mesh> m_particle_mesh;

};


#endif
