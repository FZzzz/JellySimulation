#include "Jelly.h"
#include "GLFWApp.h"
#include "ResourceManager.h"


Jelly::Jelly()
	:m_num_of_particles(0)
{
}


Jelly::~Jelly()
{
}

void Jelly::Update()
{
	/*Particle move test*/
	/*
	size_t jelly_dim = m_particles.size();
	for (size_t i = 0; i < jelly_dim; ++i)
	{
		for (size_t j = 0; j < jelly_dim; ++j)
		{
			for (size_t k = 0; k < jelly_dim; ++k)
			{
				size_t idx =
					i * (jelly_dim*jelly_dim)
					+ j * jelly_dim
					+ k;

				m_particles[i][j][k]->m_data->position += glm::vec3(0, 0.001f, 0);
			}
		}
	}
	*/
	UpdateVBO();
}

void Jelly::Initialize(std::vector<std::vector<std::vector<Particle_Ptr>>>& particles, std::vector<unsigned int> indices)
{
	auto resource_manager = GLFWApp::getInstance()->getResourceManager();
	auto shader = resource_manager->FindShaderByName("ShadowMapping");
	
	setParticles(particles);

	m_jelly_mesh = std::make_shared<Mesh>();
	m_jelly_mesh->setShader(shader);	
	m_jelly_mesh->setPositions(m_positions);
	m_jelly_mesh->setIndices(indices);
	m_jelly_mesh->SetupGLBuffers();
	
	
}

void Jelly::setParticles(std::vector<std::vector<std::vector<Particle_Ptr>>>& particles)
{
	// n^3 particles
	m_num_of_particles = particles.size() * particles.size() * particles.size();
	m_particles = particles;
	m_positions.resize(m_num_of_particles);
	/*Update member particle positions*/
	UpdateParticlePositions();
}

void Jelly::UpdateVBO()
{
	//std::vector<glm::vec3> new_positions(m_num_of_particles);
	UpdateParticlePositions();
	m_jelly_mesh->ResetPositions(m_positions);
}

void Jelly::UpdateParticlePositions()
{
	size_t jelly_dim = m_particles.size();
	for (size_t i = 0; i < jelly_dim; ++i)
	{
		for (size_t j = 0; j < jelly_dim; ++j)
		{
			for (size_t k = 0; k < jelly_dim; ++k)
			{
				size_t idx =
					i * (jelly_dim*jelly_dim)
					+ j * jelly_dim
					+ k;

				m_positions[idx] = m_particles[i][j][k]->m_data->position;
			}
		}
	}
}

