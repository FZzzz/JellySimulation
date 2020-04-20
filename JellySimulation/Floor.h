#ifndef _FLOOR_H_
#define _FLOOR_H_

#include "GameObject.h"
#include "Shader.h"
#include "Collider.h"

class Floor :
	public GameObject
{
public:
	Floor();
	~Floor();

	void Initialize(const glm::vec3& init_pos, const std::shared_ptr<Shader>& shader);

	// getters
	inline PlaneCollider* getCollider() { return m_collider; };

private:
	PlaneCollider* m_collider;
	
};

#endif