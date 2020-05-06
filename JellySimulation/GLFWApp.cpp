#include "GLFWApp.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui/imgui_impl_glfw_gl3.h"
#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#include <chrono>
#endif

#include "GameObject.h"
#include "Cube.h"
#include "Floor.h"
#include "AnimCharacter.h"
#include "Jelly.h"

void Error_callback(int error, const char* description);
void Key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void Frame_Status_GUI();
void Object_Viewer_GUI();
void Animated_Character_GUI();

GLFWApp* GLFWApp::appInstance;

GLFWApp::GLFWApp() : 
	m_frames_proccessed(0) , 
	m_previousTime(0) , 
	m_currentTime(0), 
	m_glsl_version("#version 150"),
	m_app_status(true),
	m_resource_manager(nullptr),
	m_gui_manager(nullptr),
	m_mainCamera(nullptr),
	m_renderer(nullptr)
{
}

GLFWApp::~GLFWApp()
{
	m_resource_manager.reset();
	m_mainCamera.reset();
	m_renderer.reset();
}

bool GLFWApp::Initialize(int width , int height , const std::string &title)
{
	const float f_width = static_cast<float>(width);
	const float f_height = static_cast<float>(height);

	if (!glfwInit())
	{
		return false;
	}

	glfwSetErrorCallback(Error_callback);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_SAMPLES, 4);

	/* Setting GLFW window */
	m_window = glfwCreateWindow(width, height, title.c_str() , NULL , NULL);
	
	if (!m_window)
	{
		std::cout << "Window Creation Failed!" << std::endl;
		return false;
	}

	glfwSetWindowPos(m_window, 100, 100);
	glfwMakeContextCurrent(m_window);
	glfwSetKeyCallback(m_window, Key_callback);
	glfwSwapInterval(0);

	// Initialize glew
	glewExperimental = true;
	GLenum status = glewInit();
	if (status != GLEW_OK)
	{
		std::cout << "GLEW INIT Failed! " << std::endl;
		return false;
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* ResourceManager Creation */
	m_resource_manager = std::make_shared<ResourceManager>();

	/* Simulator creation */
	m_simulator = std::make_shared<Simulation>();
	m_simulator->Initialize(PBD_MODE::XPBD);
	m_simulator->SetSolverIteration(1);
	
	// ShowdowMapping shader settings
	std::shared_ptr<Shader> shader = std::make_shared<Shader>("ShadowMapping");
	shader->SetupShader("resources/shader/shadow_mapping_vs_adv.glsl",
		"resources/shader/shadow_mapping_fs.glsl");
	
	auto mat_uniform = glGetUniformBlockIndex(shader->getProgram(), "Matrices");
	GLuint ubo;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// define the range of the buffer that links to a uniform binding point
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 3 * sizeof(glm::mat4));

	// Camera Setting
	{
		CameraDesc camera_desc;
		camera_desc.fov = 45.0f;
		camera_desc.screen_width = f_width;
		camera_desc.screen_height = f_height;
		camera_desc.near_plane = 0.001f;
		camera_desc.far_plane = 1000.0f;
		camera_desc.position = glm::vec3(0.0f, 20.0f, 60.0f);
		camera_desc.target_position = glm::vec3(0, camera_desc.position.y, 0);
		camera_desc.projection = glm::perspective(camera_desc.fov, f_width / f_height, 0.1f, 1000.0f);
		camera_desc.lookAt = glm::lookAt(camera_desc.position, camera_desc.target_position, glm::vec3(0.0f, 1.0f, 0.0f));
		camera_desc.ubo = ubo;

		//Main Camera setting
		m_mainCamera = std::make_shared<Camera>(camera_desc);
		m_resource_manager->SetMainCamera(m_mainCamera);

		//Renderer setting
		m_renderer = std::make_shared<Renderer>(m_resource_manager);
		m_renderer->Initialize(m_mainCamera, width, height);
		m_renderer->SetMainCamera(m_mainCamera);

		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferSubData(
			GL_UNIFORM_BUFFER,
			0,
			sizeof(glm::mat4),
			glm::value_ptr(m_mainCamera->m_projection));
		glBufferSubData(
			GL_UNIFORM_BUFFER,
			2 * sizeof(glm::mat4),
			sizeof(glm::mat4),
			glm::value_ptr(m_renderer->getLightMat()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// Initialize asset importer
		m_importer = std::make_shared<AssetImporter>();
	}
	// Objects setting
	//std::shared_ptr<GameObject> obj = std::make_shared<GameObject>();	
	{
		//default mesh
		auto load_status = IMPORT_STATUS::IMPORT_FAILED;
		auto mesh = m_importer->LoadMesh("resources/models/monkey.obj", shader, load_status);
		// Original
		/* 
		bool load_status = false;
		std::shared_ptr<RenderableObject> mesh = std::make_shared<RenderableObject>();
		mesh->Initialize(shader);
		load_status = mesh->LoadFromFileAssimp("resources/models/capsule.obj");
		*/
		if (load_status == IMPORT_STATUS::IMPORT_FAILED)
		{
			std::cout << "Load Failed\n";
			SignalFail();
		}
	}

	// cube mesh generation for keycall_back and resource management test
	{
		auto load_status = IMPORT_STATUS::IMPORT_FAILED;
		auto mesh = m_importer->LoadMesh("resources/models/monkey.obj", shader, load_status);
		if (load_status == IMPORT_STATUS::IMPORT_FAILED)
		{
			std::cout << "Load Failed\n";
			SignalFail();
		}
	}

	// Joint
	{
		auto load_status = IMPORT_STATUS::IMPORT_FAILED;
		auto mesh = m_importer->LoadMesh("resources/models/sphere_s.obj", shader, load_status);
		if (load_status == IMPORT_STATUS::IMPORT_FAILED)
		{
			std::cout << "Load Failed\n";
			SignalFail();
		}
	}

	// Bone Primitive
	{
		auto load_status = IMPORT_STATUS::IMPORT_FAILED;
		auto mesh = m_importer->LoadMesh("resources/models/bone_n.obj", shader, load_status);

		if (load_status == IMPORT_STATUS::IMPORT_FAILED)
		{
			std::cout << "Load Failed\n";
			SignalFail();
		}
	}
	
	
	// Terrain Initilization
	{
		std::shared_ptr<Floor> plane_terrain = std::make_shared<Floor>();
		plane_terrain->Initialize(glm::vec3(0, 0, 0), shader);
		m_resource_manager->AddGameObject(std::move(static_cast<std::shared_ptr<GameObject>>(plane_terrain)));
		m_simulator->AddCollider(plane_terrain->getCollider());
	}

	// Jelly intialization
	{
		constexpr unsigned int jelly_dim = 8;
		const float particle_mass = 0.05f;
		std::shared_ptr<Jelly> jelly = GenerateJelly(jelly_dim, particle_mass);
#ifdef _DEBUG
		assert(jelly != nullptr);
#endif
		m_resource_manager->AddJelly(jelly);
	}

	// Set colliders
	{
		glm::vec3 init_pos0(-1.f, 5, 0);
		glm::vec3 init_pos1( 1.f, 5, 0);
		float	  particle_mass = 1.f;

		Particle* p0 = new Particle(init_pos0, particle_mass);
		Particle* p1 = new Particle(init_pos1, particle_mass);
		DistanceConstraint* distance_constraint = new DistanceConstraint(p0, p1, 1.f);
		
		distance_constraint->setStiffness(0.01f);
		distance_constraint->setCompliance(0.001f);

		m_simulator->AddParticle(p0);
		m_simulator->AddParticle(p1);
		m_simulator->AddStaticConstraint(distance_constraint);
	}

	/*
		End of resources setting
	*/

	/*
		UI settings
	*/
	m_gui_manager = std::make_shared<GUIManager>();
	m_gui_manager->Initialize(m_window);
	m_gui_manager->AddGUIFunction(std::bind(Frame_Status_GUI));
	//m_gui_manager->AddGUIFunction(std::bind(Object_Viewer_GUI));
	//m_gui_manager->AddGUIFunction(std::bind(Animated_Character_GUI));
	

	m_resource_manager->ArrangeStaticObjects();

	return true;
}

void GLFWApp::Run()
{

	while (!glfwWindowShouldClose(m_window))
	{
		t0 = std::chrono::high_resolution_clock::now();
		ImGui_ImplGlfwGL3_NewFrame();

		//Frame_Status_GUI();
		//Object_Viewer_GUI();
		
		ClearBuffer();
		t1 = std::chrono::high_resolution_clock::now();
		Update();
		t2 = std::chrono::high_resolution_clock::now();
		Render();
		t3 = std::chrono::high_resolution_clock::now();

		glfwSwapBuffers(m_window);
		glfwPollEvents();
		t4 = std::chrono::high_resolution_clock::now();;
	}
	
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}

void GLFWApp::ClearBuffer()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0 , 0 , 0.35f , 0);
}

void GLFWApp::ReleaseResources()
{
	m_resource_manager->ShutDown();
}

void GLFWApp::SwitchMode()
{
	m_renderer->renderQuad = !m_renderer->renderQuad;
}

/*
void GLFWApp::CreateMonkeys(int num, OBJECT_FLAG_ENUM type)
{
	auto mesh_list = m_resource_manager->getMeshes();
	//static int name_count = 0;
	//static std::string name_pattern = "Object";
	//test add gameobject
	for (int i = 0; i < num; ++i)
	{

		float x = static_cast<float>(rand() % 10);
		float y = static_cast<float>(rand() % 10);
		float z = static_cast<float>(rand() % 10);

		auto obj = std::make_shared<GameObject>();
		obj->Initialize(glm::vec3(x, y, z));
		obj->setObjectType(type);
		obj->setMesh(mesh_list[1]);
		//obj->m_name = name_pattern + std::to_string(name_count);

		//auto mesh_list = m_resManager->getMeshList();
		//attach mesh onto game object
		//obj->setMesh(mesh_list[1]);

		obj->m_transform.m_translation = glm::vec3(x, y, z);
		if (type == OBJECT_FLAG_ENUM::OBJECT_STATIC)
			m_resource_manager->AddStaticObject(std::move(obj));
		else
			m_resource_manager->AddGameObject(std::move(obj));
		//name_count++;
	}
}
*/


std::shared_ptr<Jelly> GLFWApp::GenerateJelly(unsigned int n, float mass)
{
	if (n < 2)
		return false;

	std::shared_ptr<Jelly> jelly = std::make_shared<Jelly>();

	std::vector<std::vector<Particle_Ptr>> ps_sub(n, std::vector<Particle_Ptr>(n, nullptr));
	std::vector<std::vector<std::vector<Particle_Ptr>>> particles(n, ps_sub);
	std::vector<glm::vec3> positions;
	std::vector<unsigned int> indices;

	const float step = 2.0f / static_cast<float>(n-1);
	for (unsigned int z = 0; z < n; z++)
	{
		for (unsigned int y = 0; y < n; y++)
		{
			for (unsigned int x = 0; x < n; x++)
			{
				//size_t idx = z * n*n + y * n + x;
				float t_x = -1.0f + step * static_cast<float>(x);
				float t_y = -1.0f + step * static_cast<float>(y);
				float t_z = -1.0f + step * static_cast<float>(z);
				
				glm::vec3 pos(t_x, t_y, t_z);
				
				auto particle = std::make_shared<Particle>(pos, mass);
				particles[z][y][x] = particle;
									
				//positions.push_back(pos);
			}
		}
	}
	   
	unsigned int index = 0;
	//triangle faces
	//face 1 front
	for (unsigned int j = 0; j < n-1; j++)
	{
		for (unsigned int i = 0; i < n-1; i++)
		{
			index = n * j + i;
			indices.push_back(index);
			indices.push_back(index + 1);
			indices.push_back(index + (n+1));

			indices.push_back(index);
			indices.push_back(index + (n+1));
			indices.push_back(index + n);
		}
	}
	//face 2 back
	for (unsigned int j = 0; j < n-1; j++)
	{
		for (unsigned int i = 0; i < n-1; i++)
		{
			index = n * j + i + (n*n*(n-1));
			indices.push_back(index);
			indices.push_back(index + 1);
			indices.push_back(index + (n+1));

			indices.push_back(index);
			indices.push_back(index + (n+1));
			indices.push_back(index + n);
		}
	}
	//face 3 left 
	for (unsigned int j = 0; j < n-1; j++)
	{
		for (unsigned int i = 0; i < n-1; i++)
		{
			index = n * j + n*n * i;
			indices.push_back(index);
			indices.push_back(index + 1);
			indices.push_back(index + (n+1));

			indices.push_back(index);
			indices.push_back(index + (n+1));
			indices.push_back(index + n);
		}
	}
	//face 4 right
	for (unsigned int j = 0; j < n-1; j++)
	{
		for (unsigned int i = 0; i < n-1; i++)
		{
			index = n * j + n*n * i + (n-1);
			indices.push_back(index);
			indices.push_back(index + 1);
			indices.push_back(index + (n+1));

			indices.push_back(index);
			indices.push_back(index + (n+1));
			indices.push_back(index + n);
		}
	}
	//face 5 top 
	for (unsigned int j = 0; j < n-1; j++)
	{
		for (unsigned int i = 0; i < n-1; i++)
		{
			index = n*n * j + i;
			indices.push_back(index);
			indices.push_back(index + 1);
			indices.push_back(index + (n+1));

			indices.push_back(index);
			indices.push_back(index + (n+1));
			indices.push_back(index + n);
		}
	}
	//face 6 buttom
	for (unsigned int j = 0; j < n-1; j++)
	{
		for (unsigned int i = 0; i < n-1; i += 1)
		{
			index = n*n * j + i + n*(n-1);
			indices.push_back(index);
			indices.push_back(index + 1);
			indices.push_back(index + (n+1));

			indices.push_back(index);
			indices.push_back(index + (n+1));
			indices.push_back(index + n);
		}
	}

	jelly->Initialize(particles, indices);

	return jelly;
}

void GLFWApp::SignalFail()
{
	m_app_status = false;
}

float GLFWApp::getElapsedTime()
{
	return static_cast<float>(glfwGetTime());
}

void Error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

void Key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	static GLFWApp* const instance = GLFWApp::getInstance();
	static auto res_manager = instance->getResourceManager();
	static auto camera = res_manager->getMainCamera();
	if (action == GLFW_RELEASE)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		}

		case GLFW_KEY_H:
		{
			instance->SwitchMode();
			break;
		}

		case GLFW_KEY_G:
		{
			static int name_count = 0;
			static std::string name_pattern = "Object";
			//test add gameobject
			for (int i = 0; i < 10000; ++i)
			{
				
				float x = static_cast<float>(rand() % 10);
				float y = static_cast<float>(rand() % 10);
				float z = static_cast<float>(rand() % 10);
				
				std::shared_ptr<GameObject> obj = std::make_shared<Cube>();

				obj->Initialize(glm::vec3(x,y,z));
				obj->setName(name_pattern + std::to_string(name_count));
				obj->m_transform.m_translation = glm::vec3(x, y, z);

				instance->getResourceManager()->AddGameObject(std::move(obj));
				name_count++;
			}
			break;
		}

		case GLFW_KEY_SPACE:
		{
			auto simulator = instance->getSimulator();
			simulator->Pause();
			//simulator->Step(0.0001f);
			break;
		}
		
		case GLFW_KEY_BACKSPACE:
		{
			for (int i = 0; i < 1; ++i)
			{
				//test memory release
				instance->getResourceManager()->RemoveObjectFromLast();
			}
			break;
		}

		}
	}
	else if (action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_UP:
		{
			camera->Zoom(-0.25f);
			break;
		}

		case GLFW_KEY_DOWN:
		{
			camera->Zoom(0.25f);
			break;
		}
		case GLFW_KEY_RIGHT:
		{
			camera->Rotate(0.2f / 30.0f);
			break;
		}

		case GLFW_KEY_LEFT:
		{
			camera->Rotate(-0.2f / 30.0f);
			break;
		}
		}
	}

}

void GLFWApp::Render()
{
	m_renderer->Render();
	//GUI rendering
	m_gui_manager->Render();
}

void GLFWApp::Update()
{
	auto anim_characters = m_resource_manager->getAnimCharacters();
	auto jellies = m_resource_manager->getJellies();

	m_previousTime = m_currentTime;
	m_currentTime = static_cast<float>(glfwGetTime());
	
	float dt = m_currentTime - m_previousTime;

	m_simulator->Step(0.001f);

	for (auto it = anim_characters.cbegin(); it != anim_characters.cend(); ++it)
	{
		(*it)->Update();
	}

	for (auto jelly : jellies)
	{
		jelly->Update();
	}

	
	for (auto it = m_resource_manager->getObjects().begin(); 
		it != m_resource_manager->getObjects().end(); ++it)
	{
		(*it)->Update();
	}

	m_mainCamera->Update();

	// GUI update
	m_gui_manager->Update();
}

void GLFWApp::SetUpImGui()
{
	//Set up imgui binding
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfwGL3_Init(m_window, false);
	ImGui::StyleColorsDark();
}

void Frame_Status_GUI()
{
	auto resource_manager = GLFWApp::getInstance()->getResourceManager();
	auto camera = resource_manager->getMainCamera();
	// If we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
	ImGui::Begin("Frame Status");
	static float f = 0.0f;
	static int counter = 0;
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::NewLine();
	ImGui::Text("Object Array Size: %u", resource_manager->getObjects().size());
	ImGui::Text("Mesh Array Size: %u", resource_manager->getMeshes().size());
	ImGui::Text("Shader Array Size: %u", resource_manager->getShaders().size());
	//ImGui::Text("IMGUI time: %.2lf ms", (t0-t4).count()/1000000.0);
	//ImGui::Text("Update time: %.2lf ms", (t2-t1).count()/1000000.0);
	//ImGui::Text("Render time: %.2lf ms", (t3-t2).count()/1000000.0);
	//ImGui::Text("SWAP & PollEvent time: %.2lf ms", (t4-t3).count()/1000000.0);

	ImGui::Text("Camera Position: %.2f, %.2f, %.2f", camera->m_position.x, camera->m_position.y, camera->m_position.z);

	ImGui::End();
}

void Object_Viewer_GUI()
{
	ImGui::Begin("Object List");
	auto resource_manager = GLFWApp::getInstance()->getResourceManager();
	
	static int update_count = 0;
	static std::string name_list = "";
	if (update_count % 60 == 0)
	{
		update_count = 0;
		name_list.clear();
		for (auto it = resource_manager->getObjects().begin();
			it != resource_manager->getObjects().end(); ++it)
		{
			name_list += (*it)->getName() + "\n";
		}
	}
	ImGui::Text("%s", name_list.data());
	ImGui::NewLine();
	
	update_count++;
	ImGui::End();
}

void Animated_Character_GUI()
{
	ImGui::Begin("Animated Character Panel");

	auto resource_manager = GLFWApp::getInstance()->getResourceManager();
	auto anim_characters = resource_manager->getAnimCharacters();
	
	enum Channel
	{
		Channel_RX,
		Channel_RY,
		Channel_RZ
	};

	for (auto it = anim_characters.cbegin();
		it != anim_characters.cend();
		++it)
	{
		auto character = *it;
		auto anim = character->getAnimation();
		auto& root_transform = character->getRoot()->m_transform;
		if (ImGui::CollapsingHeader(character->getName().c_str()))
		{
			float v[3] = { root_transform.m_translation.x, root_transform.m_translation.x, root_transform.m_translation.z };
			ImGui::InputFloat3("Root Position", v, 3, ImGuiInputTextFlags_ReadOnly);

			int current_frame_number = static_cast<int>(anim->getCurrentFrameTime());
			ImGui::SliderInt("Frame number", &current_frame_number, 0, anim->getKeyFrameSize() - 1);
			anim->setCurrentFrame(static_cast<size_t>(current_frame_number));
		}
	}
	
	ImGui::End();
}

