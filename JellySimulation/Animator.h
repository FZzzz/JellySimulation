#ifndef _ANIMATOR_H_
#define _ANIMATOR_H_

#include <glm/glm.hpp>
#include <vector>
/*
	TODO:
		< Implement position interpolations >
		< Implement rotation interpolations >
		1. Load Functions
		2. linear interpolation
		3. curve interpolation
		4. Test
*/

struct FrameState
{
	glm::vec3 position;
};

struct Keyframe
{
	FrameState frame;
	int key; //count
};

enum InterpolationType
{
	INTERPOLATE_LINEAR ,
	INTERPOLATE_CUBIC_BEZIER
};


class Animator
{
public:
	Animator();
	~Animator();

	inline void setInterpolationType(InterpolationType theType);

	void Initialize();
	void Load();

	inline const FrameState& getCurrentFrameState();


	//Control Vector => ( (deltaPos) , (deltaTime) )
	//End points => depend on key-frames
	void setControlVectors(glm::vec4 p1 , glm::vec4 p2);

private:

	void InterpolateFrame();
	//in next patch will use framesate as the input reference
	void interpolateAsBezierCurve(const glm::vec3& p1 , const glm::vec3& p2 , float a,  glm::vec3& result);

	InterpolationType m_type;
	bool m_isLoop;
	int  m_count;
	int  m_totalFrameCount;
	unsigned int m_currentKeyIndex;
	FrameState m_currentFrame;

	std::vector<Keyframe> m_keyframes;

	glm::vec4 m_controlPoint1 , m_controlPoint2;

};

#endif
