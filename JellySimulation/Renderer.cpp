#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <chrono>
#include <algorithm>

#define SHADOW_WIDTH  1024
#define SHADOW_HEIGHT 1024

Renderer::Renderer(std::shared_ptr<ResourceManager> resource_manager) : 
				m_resource_manager(resource_manager), 
				light_mat(glm::mat4(1)), m_mainCamera(nullptr), 
				m_screenHeight(0),
				m_screenWidth(0),
				m_depthMapFBO(0),
				m_depthMap(0),
				m_simpleDepthShader(nullptr),
				m_quad_shader(nullptr),
				quadVAO(0),
				quadVBO(0)
				
{
}

Renderer::~Renderer()
{
}

void Renderer::Initialize(std::shared_ptr<Camera> camera, int screenWidth, int screenHeight)
{
	m_mainCamera = camera;
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	m_simpleDepthShader = std::make_shared<Shader>("SimpleDepthShader");

	m_simpleDepthShader->SetupShader("resources/shader/simple_depth_vs.glsl", 
									 "resources/shader/simple_depth_fs.glsl");

	m_quad_shader = std::make_shared<Shader>("DebugQuadShader");
	m_quad_shader->SetupShader("resources/shader/debug_quad_vs.glsl",
							   "resources/shader/debug_quad_fs.glsl");


	float near_plane = 0.1f, far_plane = 100.f;
	glm::mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

	glm::mat4 lightView = glm::lookAt(m_mainCamera->m_position,
									  glm::vec3(0.0f, 0.0f, 0.0f),
									  glm::vec3(0.0f, 1.0f, 0.0f));

	light_mat = light_projection * lightView;

	//Generate shadow framebuffer
	glGenFramebuffers(1, &m_depthMapFBO);
	
	glGenTextures(1, &m_depthMap);
	glBindTexture(GL_TEXTURE_2D, m_depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//Generate depth texture
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
}

void Renderer::SetShaderObjectMap(std::shared_ptr<Shader> shader, std::shared_ptr<GameObject> object)
{
	if (m_shader_object_map.find(shader) != m_shader_object_map.end())
	{
		// Append
		m_shader_object_map[shader].push_back(object);
	}
	else
	{
		// Create new vecotr
		std::vector<std::shared_ptr<GameObject>> vec;
		vec.push_back(object);
		m_shader_object_map.emplace(shader, vec);
	}

}

void Renderer::RemoveObjectMapCache(std::shared_ptr<Shader> shader, std::shared_ptr<GameObject> object)
{
	auto map_it = m_shader_object_map.find(shader);
	auto& obj_vec = map_it->second;
	auto obj_it = std::find(obj_vec.begin(), obj_vec.end(), object);
	if (obj_it != obj_vec.end())
	{
		obj_vec.erase(obj_it);
	}
	
}

void Renderer::Render()
{
	// No camera, no render needed
	if (m_mainCamera == nullptr)
	{
		return;
	}

	RenderSceneDepth();
	if (!renderQuad)
	{
		RenderShadowMappedScene();
	}
	else
	{
		RenderQuad();
	}
}

void Renderer::RenderSceneDepth()
{
	// 1. first render to depth map
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	//ConfigureShaderAndMatrices();

	// depth shading
	m_simpleDepthShader->Use();

	const auto& static_object_vec = m_resource_manager->getStaticObjects();
	for (size_t i = 0; i < static_object_vec.size(); ++i)
	{
		if (static_object_vec[i]->hasMesh())
		{
			RenderDepth(static_object_vec[i]->getMesh(), (static_object_vec[i]->m_transform));//Render the mesh
		}
	}

	const std::vector<std::shared_ptr<GameObject>>& objects = m_resource_manager->getObjects();
	for (size_t i = 0; i < objects.size(); i++)
	{
		if (objects[i]->hasMesh())
		{
			RenderDepth(objects[i]->getMesh(), (objects[i]->m_transform));//Render the mesh
		}
	}

	auto jellies = m_resource_manager->getJellies();
	for (auto jelly : jellies)
	{
		if (jelly->getJellyMesh())
		{
			jelly->getJellyMesh()->getShader()->Use();
			// jelly's position is operated by vertex position;
			RenderDepth(jelly->getJellyMesh(), Transform());
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::RenderShadowMappedScene()
{
	// 2. then render scene as normal with shadow mapping (using depth map)
	glViewport(0, 0, m_screenWidth, m_screenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//ConfigureShaderAndMatrices();
	RenderScene();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::RenderParticles(const std::vector<Particle_Ptr> &particles)
{
	if (!m_mainCamera)
		return;

	auto shader = m_resource_manager->FindShaderByName("PointSprite");
	shader->Use();

	const glm::mat4 pvm = m_mainCamera->m_cameraMat * glm::mat4(1);
	shader->SetUniformMat4("pvm", pvm);
	shader->SetUniformFloat("point_size", 5.f);
	shader->SetUniformVec3("light_pos", m_mainCamera->m_position);
	shader->SetUniformMat4("view", m_mainCamera->m_lookAt);
	glBindVertexArray(m_particles_VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_particles_EBO);
	glDrawElements(
		GL_POINTS,
		static_cast<unsigned int>(particles.size()),
		GL_UNSIGNED_INT,
		0
	);
	glBindVertexArray(0);
}

void Renderer::RenderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}


	m_quad_shader->Use();
	const auto& shader_program = m_quad_shader->getProgram();
	m_quad_shader->SetUniformInt("depthMap", 0);
	//GLFunctions::SetUniformInt("depthMap", 0, shader_program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_depthMap);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

}

void Renderer::RenderScene()
{
	auto particles = m_resource_manager->getParticles();
	
	//auto t1 = std::chrono::high_resolution_clock::now();
	const std::vector<std::shared_ptr<GameObject>>& objects = m_resource_manager->getObjects();
	
	auto jellies = m_resource_manager->getJellies();
	
	
	// For each shader program we call object->render()
	// Our render will automatically arrange the shader->objects map
	for (auto it = m_shader_object_map.cbegin(); it != m_shader_object_map.cend(); ++it)
	{
		auto shader = it->first;
		const auto& obj_vec = it->second;
		
		shader->Use();
		for (auto obj_it = obj_vec.cbegin(); obj_it != obj_vec.cend(); ++obj_it)
		{
			RenderShadowMappingObject(shader, (*obj_it)->getMesh(), (*obj_it)->m_transform);
		}
	}

	for (auto jelly : jellies)
	{
		if (jelly->getJellyMesh())
		{
			// jelly's position is operated by vertex position;
			RenderShadowMappingObject(jelly->getJellyMesh()->getShader(), jelly->getJellyMesh(), Transform());
		}
	}
	
	
	RenderParticles(particles);
	/*
	// The above code equals to the below one
	for (i = 0; i < objects.size(); i++)
	{
		if (objects[i]->hasMesh())
		{
			RenderObject(objects[i]->getMesh(), *(objects[i]->m_transform));//Render the mesh
		}
	}
	*/

}

void Renderer::RenderShadowMappingObject(const std::shared_ptr<Shader>& shader, 
							const std::shared_ptr<Mesh>& mesh , 
							const Transform& transform)
{
	if (!m_mainCamera)
		return;

	if (!mesh)
	{
		std::cout << "No Mesh to Render\n";
		return;
	}
	//mesh->Render();

	//shadow mapping shader
	const glm::mat4 pvm = m_mainCamera->m_cameraMat * transform.getModelMatWorld();
	//shader->SetUniformMat4("pvm", pvm);
	shader->SetUniformMat4("modelMat", transform.getModelMatWorld());
	//shader->SetUniformMat4("lightSpaceMatrix", light_mat);
	
	// shadow map configuration
	shader->SetUniformInt("shadowMap", 0);
	shader->SetUniformVec3("lightPos", m_mainCamera->m_position);
	shader->SetUniformVec3("viewPos", m_mainCamera->m_position);

	//glBindTexture(GL_TEXTURE_2D, m_depthMap);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_depthMap);
	
	glBindVertexArray(mesh->getVAO());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->getEBO());
	glDrawElements(
		GL_TRIANGLES, 
		static_cast<unsigned int>(mesh->getNumberOfIndices()), 
		GL_UNSIGNED_INT, 
		0);
	glBindVertexArray(0);

}

void Renderer::RenderInstancedObject(const std::shared_ptr<Mesh>& mesh, const Transform & transform)
{
	if (!m_mainCamera)
		return;

	if (!mesh)
	{
		std::cout << "No Mesh to Render\n";
		return;
	}
	mesh->Render();

	//shadow mapping shader
	const auto& shader_program = mesh->getShaderProgram();
	const auto& shader = mesh->getShader();

	//auto t1 = std::chrono::high_resolution_clock::now();
	//const glm::mat4& pvm = m_mainCamera->projection * m_mainCamera->lookAt * transform.getModelMat();
	const glm::mat4 pvm = m_mainCamera->m_cameraMat * transform.getModelMatWorld();
	//auto t2 = std::chrono::high_resolution_clock::now();
	//std::cout << (t2 - t1).count() << "\n";
	shader->SetUniformMat4("pvm", pvm);
	shader->SetUniformMat4("modelMat", transform.getModelMatWorld());
	shader->SetUniformMat4("lightSpaceMatrix", light_mat);

	// shadow map configurations
	shader->SetUniformInt("shadowMap", 0);
	shader->SetUniformVec3("lightPos", m_mainCamera->m_position);
	shader->SetUniformVec3("viewPos", m_mainCamera->m_position);

	//Depth texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_depthMap);

	glBindVertexArray(mesh->getVAO());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->getEBO());
	glDrawElements(
		GL_TRIANGLES,
		static_cast<unsigned int>(mesh->getNumberOfIndices()),
		GL_UNSIGNED_INT,
		0);
	glBindVertexArray(0);
}

void Renderer::RenderDepth(const std::shared_ptr<Mesh>& mesh, const Transform& transform)
{
	if (!m_mainCamera && !m_simpleDepthShader)
		return;

	if (!mesh)
	{
		std::cout << "No Mesh to Render\n";
		return;
	}
	
	//auto shader_program = mesh_sp->getShaderProgram();

	const glm::mat4 pvm = m_mainCamera->m_cameraMat * transform.getModelMatWorld();
	
	m_simpleDepthShader->SetUniformMat4("lightSpaceMatrix", light_mat);
	m_simpleDepthShader->SetUniformMat4("modelMat", transform.getModelMatWorld());
	//GLFunctions::SetUniformMat4("lightSpaceMatrix", light_mat, shader_program);
	//GLFunctions::SetUniformMat4("modelMat", transform.getModelMatWorld(), shader_program);

	glBindVertexArray(mesh->getVAO());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->getEBO());
	glDrawElements(
		GL_TRIANGLES, 
		static_cast<unsigned int>(mesh->getNumberOfIndices()), 
		GL_UNSIGNED_INT, 
		0);
	glBindVertexArray(0);
}

void Renderer::SetMainCamera(std::shared_ptr<Camera> camera)
{
	m_mainCamera = camera;
}

void Renderer::SetupParticleGLBuffers(const std::vector<Particle_Ptr>& particles)
{
	std::vector<glm::vec3> positions;
	for (auto particle : particles)
	{
		positions.push_back(particle->m_data->position);
	}

	auto shader = m_resource_manager->FindShaderByName("PointSprite");
	shader->Use();
	glGenVertexArrays(1, &m_particles_VAO);
	glGenBuffers(1, &m_particles_VBO);
	glGenBuffers(1, &m_particles_EBO);

	// Update VBOs
	glBindVertexArray(m_particles_VAO);

	//upload vertices position to GPU
	if (particles.size() > 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_particles_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(),
			positions.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	std::vector<GLuint> indices;
	//setup indices info
	for (GLuint i = 0; i < particles.size(); ++i)
	{
		indices.push_back(i);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_particles_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * particles.size(),
		indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);

}

