#include "Particle.h"
#include "imgui/imgui.h"
#include "CollisionDetection.h"

Particle::Particle(glm::vec3 pos, float mass)
	:m_collider(new PointCollider(pos))
{
	m_data = new ParticleData(pos, mass);
}

Particle::~Particle()
{
	if(m_data) delete m_data;
	delete m_collider;

	m_collider = nullptr;
	m_data = nullptr;
}

void Particle::Update(float dt)
{
	m_data->prev_force = m_data->force;
	m_data->prev_velocity = m_data->velocity;
	m_data->prev_position = m_data->position;
	
	m_data->force = m_data->new_force;
	
	/* This is why it is called "Position Based Dynamics" */
	m_data->velocity = (m_data->new_position - m_data->position) / dt;
	m_data->position = m_data->new_position;

	UpdateCollider();
}

void Particle::UpdateCollider()
{
	m_collider->m_position = m_data->position;
}

bool Particle::TestCollision(Collider* other)
{
	bool result = false;
	switch (other->getColliderTypes())
	{
	case Collider::ColliderTypes::SPHERE:
		result = CollisionDetection::PointSphereIntersection(
			m_data->new_position, dynamic_cast<SphereCollider*>(other));
		break;

	case Collider::ColliderTypes::AABB:
		result = CollisionDetection::PointAABBIntersection(
			m_data->new_position, dynamic_cast<AABB*>(other)
		);
		break;

	case Collider::ColliderTypes::OBB:
		result = CollisionDetection::PointOBBIntersection(
			m_data->new_position, dynamic_cast<OBB*>(other)
		);
		break;

	case Collider::ColliderTypes::PLANE:
		result = CollisionDetection::PointPlaneIntersection(
			m_data->new_position, dynamic_cast<PlaneCollider*>(other)
		);
		break;
	}
	return result;
}

void Particle::OnCollision(Collider* other)
{
	/* Collision response variables */
	glm::vec3 normal;
	glm::vec3 v_r;
	
	   
	switch (other->getColliderTypes())
	{
	case Collider::ColliderTypes::SPHERE: 
	{
		SphereCollider* sphere = dynamic_cast<SphereCollider*>(other);
		normal = m_data->new_position - sphere->m_center;

		if (glm::dot(normal, normal) == 0)
			v_r = m_data->velocity;
		else
		{
			normal = glm::normalize(normal);
			v_r = m_data->velocity - 2 * glm::dot(v_r, normal) * normal;
		}
		break;
	}

	case Collider::ColliderTypes::AABB:
	{
		AABB* aabb = dynamic_cast<AABB*>(other);

		glm::vec3 diff2max = aabb->m_max - m_data->new_position;
		glm::vec3 diff2min = m_data->new_position - aabb->m_min;

		glm::vec3 normal_preset[6] =
		{
			glm::vec3(1,0,0),
			glm::vec3(0,1,0),
			glm::vec3(0,0,1),
			glm::vec3(-1,0,0),
			glm::vec3(0,-1,0),
			glm::vec3(0,0,-1),
		};

		float min_dist = diff2max.x;
		normal = normal_preset[0];

		// test 6 distance
		for (int i = 1; i < 3; ++i)
		{
			if(min_dist < diff2max[i])
				min_dist = diff2max[i], normal = normal_preset[i];
		}

		for (int i = 0; i < 3; ++i)
		{
			if (min_dist < diff2min[i])
				min_dist = diff2min[i], normal = normal_preset[3 + i];
		}

		v_r = m_data->velocity - 2 * glm::dot(v_r, normal) * normal;
		m_data->new_velocity = v_r;

		break;
	}
	
	case Collider::ColliderTypes::OBB:
	{
		/*
			TODO: Implement this
		*/
		break;
	}

	case Collider::ColliderTypes::PLANE:
	{
		PlaneCollider* plane = dynamic_cast<PlaneCollider*>(other);
		normal = plane->m_normal;
		v_r = m_data->velocity - 2 * glm::dot(v_r, normal) * normal;
		m_data->new_velocity = v_r;

		break;
	}
	}
}
