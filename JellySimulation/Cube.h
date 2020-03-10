#ifndef _CUBE_H_
#define _CUBE_H_

#include "GameObject.h"
class Cube : public GameObject
{
public:
	Cube();
	~Cube();

	void Initialize(const glm::vec3& position) override;
	void Update() override;

};

#endif