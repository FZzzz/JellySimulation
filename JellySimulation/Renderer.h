#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <vector>
#include <memory>
#include <unordered_map>
#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"
#include "ResourceManager.h"


/*
	Renderer only handle process of render and renderable resources
*/

using Shader_Object_Map = std::unordered_map< std::shared_ptr<Shader>, std::vector<std::shared_ptr<GameObject>> >;

class Renderer
{
public:
	Renderer(std::shared_ptr<ResourceManager> resource_manager);
	~Renderer();

	void Initialize(std::shared_ptr<Camera> camera,int screenWidth, int screenHeight);

	void SetShaderObjectMap(std::shared_ptr<Shader> shader, std::shared_ptr<GameObject> object);
	void RemoveObjectMapCache(std::shared_ptr<Shader> shader, std::shared_ptr<GameObject> object);

	void Render();
	
	void SetMainCamera(std::shared_ptr<Camera> camera);

	bool renderQuad = false;

	inline const glm::mat4& getLightMat() { return light_mat; };
	inline Shader_Object_Map& getShaderObjectMap() { return m_shader_object_map; };
	
private:

	void RenderScene();
	//void RenderStaticObject();
	//void RenderInstancedObject();
	void RenderObject(
		const std::shared_ptr<Shader>& shader,
		const std::shared_ptr<Mesh>& mesh, 
		const Transform& transform);
	void RenderInstancedObject(const std::shared_ptr<Mesh>& mesh, const Transform& transform);
	void RenderDepth(const std::shared_ptr<Mesh>& mesh, const Transform& transform);
	void RenderSceneDepth();
	void RenderSceneWithShadowMap();

	void RenderParticles();

	void RenderQuad();

	std::shared_ptr<ResourceManager> m_resource_manager;
	Shader_Object_Map m_shader_object_map;

	glm::mat4 light_mat;
	std::shared_ptr<Camera> m_mainCamera;
	
	int m_screenWidth;
	int m_screenHeight;

	// GL buffers
	GLuint m_depthMapFBO;
	GLuint m_depthMap;
	GLuint m_indirectBuffer;
	GLuint m_indirect_VAO;
	GLuint m_indirect_VBO;
	GLuint m_indirect_EBO;
		
	
	
	std::shared_ptr<Shader> m_simpleDepthShader;
	std::shared_ptr<Shader> m_quad_shader;

	GLuint quadVAO, quadVBO;

};

#endif
