#include "Cube.h"
#include "GLFWApp.h"
#include <cstdlib>
#include <ctime>

Cube::Cube() : GameObject()
{
	
}

Cube::~Cube()
{
}

void Cube::Initialize(const glm::vec3& position)
{
	m_transform.m_translation = position;
	GameObject::Initialize();
	//GameObject::Initialize(position);
	auto res_manager = GLFWApp::getInstance()->getResourceManager();
	auto mesh_list = res_manager->getMeshes();

	setMesh(mesh_list[1]);
}

void Cube::Update()
{
	/*random move*/
	/*
	float dx = static_cast<float>(rand() % 100)/1000.0f;
	float dy = static_cast<float>(rand() % 100)/1000.0f;
	float dz = static_cast<float>(rand() % 100)/1000.0f;

	//this->m_transform->m_translation += glm::vec3(dx, dy, dz);
	*/
	GameObject::Update();

}
