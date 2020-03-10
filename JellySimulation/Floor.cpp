#include "Floor.h"
#include "GLFWApp.h"

Floor::Floor()
{
}

Floor::~Floor()
{
}

void Floor::Initialize(const glm::vec3& init_pos, const std::shared_ptr<Shader>& shader)
{

	//std::shared_ptr<Shader> shader = std::make_shared<Shader>();
	//shader->SetupShader("resources/shader/shadow_mapping_vs.glsl",
	//					"resources/shader/shadow_mapping_fs.glsl");

	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	mesh->Initialize(shader);	   

	std::vector<glm::vec3> vertex_pos;
	vertex_pos.reserve(4);
	vertex_pos.push_back(glm::vec3(  100.0f, 0.0f, -100.0f));
	vertex_pos.push_back(glm::vec3(  100.0f, 0.0f,  100.0f));
	vertex_pos.push_back(glm::vec3( -100.0f, 0.0f,  100.0f));
	vertex_pos.push_back(glm::vec3( -100.0f, 0.0f, -100.0f));

	std::vector<unsigned int> indices;
	vertex_pos.reserve(6);
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(3);
	
	mesh->setPositions(vertex_pos);
	mesh->setIndices(indices);
	mesh->SetupGLBuffers();

	setMesh(mesh);

	// Not use primitive box geometry,
	// Physics Engine has already created one for plane terrain
	GameObject::Initialize(init_pos);

}

