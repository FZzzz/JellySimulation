#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Camera::Camera(CameraDesc desc) : 
	m_projection(desc.projection),
	m_lookAt(desc.lookAt),
	m_position(desc.position),
	m_fov(desc.fov),
	m_screen_width(desc.screen_width),
	m_screen_height(desc.screen_height),
	m_near_plane(desc.near_plane),
	m_far_plane(desc.far_plane),
	m_camera_ubo(desc.ubo),
	m_rotate_radius(m_position.z),
	m_camera_height(m_position.y),
	m_target_position(desc.target_position)
{
}

Camera::Camera(glm::mat4 projection, glm::mat4 lookAt, glm::vec3 position, GLuint ubo)
	: m_lock_on_mode(false),
	  m_camera_ubo(ubo),
	  m_theta(0.0f)
{
	this->m_projection = projection;
	this->m_lookAt = lookAt;
	this->m_position = position;
	m_cameraMat = projection * lookAt;
}

Camera::~Camera()
{
}

void Camera::Zoom(float change)
{
	//fov += change;
	//projection = glm::perspective(fov, screen_width / screen_height, 0.1f, 100.0f);
	m_rotate_radius += change;
	m_position = glm::vec3(m_rotate_radius * glm::sin(m_theta), m_camera_height, m_rotate_radius * glm::cos(m_theta));
	m_lookAt = glm::lookAt(m_position, m_target_position, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::Rotate(float change)
{
	this->m_theta += change;
	m_position = glm::vec3(m_rotate_radius * glm::sin(m_theta), m_camera_height, m_rotate_radius * glm::cos(m_theta));
	m_lookAt = glm::lookAt(m_position, m_target_position, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::Update()
{
	if (m_camera_ubo != -1)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_camera_ubo);
		glBufferSubData(
			GL_UNIFORM_BUFFER,
			0,
			sizeof(glm::mat4),
			glm::value_ptr(m_projection));
		glBufferSubData(
			GL_UNIFORM_BUFFER,
			sizeof(glm::mat4),
			sizeof(glm::mat4),
			glm::value_ptr(m_lookAt));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	m_cameraMat = m_projection * m_lookAt;
		
	/*
	if (!lock_on_mode)
		return;
	position = lookAtObj->m_mesh->m_tranlation + glm::vec3(0, 10, -5);

	const glm::vec3 up = glm::vec3(0, 1, 0);
	const glm::vec3 front = lookAtObj->m_mesh->m_tranlation - position;
	const glm::vec3 right = glm::normalize(glm::cross(up , front));
	const glm::vec3 camUp = glm::cross(front, right);

	lookAt = glm::lookAt(position , position + front , camUp);
	*/
}
