#ifndef _GLFWAPP_H
#define _GLFWAPP_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <glm/glm.hpp>
#include <memory>
#include <chrono>
#include <unordered_map>
#include "imgui/imgui.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "Renderer.h"
#include "AnimCharacter.h"
#include "AssetImporter.h"
#include "GUIManager.h"
#include "Jelly.h"
#include "Simulation.h"

class GLFWApp
{
public:
	GLFWApp();
	~GLFWApp();
	
	bool Initialize(int width , int height , const std::string &title);
	void Run();
	void ClearBuffer();
	void ReleaseResources();
	void SwitchMode();
	//void CreateMonkeys(int num, OBJECT_FLAG_ENUM type);
	std::shared_ptr<Jelly> GenerateJelly(unsigned int n, float mass, float half_width, glm::vec3 init_pos);

	
	static GLFWApp* getInstance() {
		if (!appInstance)
			appInstance = new GLFWApp();
		return appInstance; 
	};
	inline std::shared_ptr<ResourceManager> getResourceManager() { return m_resource_manager; }
	inline std::shared_ptr<GUIManager> getGUIManager() { return m_gui_manager; }
	inline const std::shared_ptr<Renderer> getRenderer() { return m_renderer; }
	inline const std::shared_ptr<Simulation> getSimulator() { return m_simulator; }
	inline bool GetAppStatus() { return m_app_status; };
	
	/*virtual functions*/
	virtual float getElapsedTime();


private:
	
	void Render();
	void Update();
	void SignalFail();
	void SetUpImGui();
	void GenerateStructuralSprings(
		std::vector<std::vector<std::vector<Particle_Ptr>>>& particles,
		unsigned int x, unsigned int y, unsigned int z,
		const float spring_length,
		const float stiffness,
		const float compliance,
		const unsigned int n);

	void GenerateShearSprings(
		std::vector<std::vector<std::vector<Particle_Ptr>>>& particles,
		unsigned int x, unsigned int y, unsigned int z,
		const float spring_length,
		const float stiffness,
		const float compliance,
		const unsigned int n);

	void GenerateBendSprings(
		std::vector<std::vector<std::vector<Particle_Ptr>>>& particles,
		unsigned int x, unsigned int y, unsigned int z,
		const float spring_length,
		const float stiffness,
		const float compliance,
		const unsigned int n);

	void GenerateRadomParticles();

	static GLFWApp* appInstance;

	GLFWwindow* m_window;
	GLFWcursor* m_mouseCursors[ImGuiMouseCursor_COUNT];

	double m_previousTime , m_currentTime , m_deltaTime;
	int m_frames_proccessed;
	char m_glsl_version[32];
	
	/*Importer*/
	std::shared_ptr<AssetImporter> m_importer;

	/*Manager*/
	std::shared_ptr<ResourceManager> m_resource_manager;
	std::shared_ptr<GUIManager> m_gui_manager;
	
	/*Resources*/
	std::shared_ptr<Camera> m_mainCamera;
	std::shared_ptr<Renderer> m_renderer;
	
	/* Timer */
	std::chrono::high_resolution_clock::time_point t0, t1, t2, t3, t4;

	/*Simulation*/
	std::shared_ptr<Simulation> m_simulator;

	/*Others*/
	std::shared_ptr<AnimCharacter> m_anim_character;

	/*GUIs*/
	//void Frame_Status_GUI();
	//void Object_Viewer_GUI();

	bool m_app_status;
};

#endif
