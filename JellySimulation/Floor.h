#ifndef _FLOOR_H_
#define _FLOOR_H_

#include "GameObject.h"
#include "Shader.h"
class Floor :
	public GameObject
{
public:
	Floor();
	~Floor();

	void Initialize(const glm::vec3& init_pos, const std::shared_ptr<Shader>& shader);
	
};

#endif