#include "Animator.h"
#include <glm/gtx/compatibility.hpp>

Animator::Animator() :m_type(INTERPOLATE_LINEAR) , m_isLoop (false) , m_count(0), m_totalFrameCount(0), m_currentKeyIndex(0)
{
}


Animator::~Animator()
{
}

inline void Animator::setInterpolationType(InterpolationType theType)
{
	m_type = theType;
}

void Animator::Initialize()
{
	m_currentFrame.position = glm::vec3(0, 0, 0);
}

void Animator::Load()
{
	/*
		load keyframes
	*/
}

inline const FrameState& Animator::getCurrentFrameState()
{
	m_count++;

	if (m_count > m_totalFrameCount)
	{
		m_count = m_isLoop ? 0 : m_totalFrameCount;
	}

	InterpolateFrame();
	return m_currentFrame;
}

void Animator::setControlVectors(glm::vec4 p1 , glm::vec4 p2)
{
	/*
		handle invalid control vectors
	*/
	m_controlPoint1 = p1;
	m_controlPoint2 = p2;
}


void Animator::InterpolateFrame()
{
	/*
		Interpolate between two keyframe
	*/
	if (m_count == m_keyframes[m_currentKeyIndex].key)
	{
		m_currentFrame = m_keyframes[m_currentKeyIndex].frame;
		return;
	}

	const glm::vec3 p0 = m_keyframes[m_currentKeyIndex].frame.position;
	const glm::vec3 p1 = m_keyframes[m_currentKeyIndex + 1].frame.position;
	const int current_key = m_keyframes[m_currentKeyIndex].key;
	const int next_key = m_keyframes[m_currentKeyIndex + 1].key;
	const float a = static_cast<float>(m_count - current_key) / static_cast<float>(next_key - current_key);

	switch (m_type)
	{
		case INTERPOLATE_LINEAR:
		{
			m_currentFrame.position = glm::lerp(p0, p1, a);
			break;
		}
		case INTERPOLATE_CUBIC_BEZIER:
		{
			interpolateAsBezierCurve(p0, p1, a, m_currentFrame.position);
			break;
		}
		default:
			break;
	}

}

void Animator::interpolateAsBezierCurve(const glm::vec3& p1 , const glm::vec3& p2 , float a,  glm::vec3& result)
{
	const glm::vec4 p1_vec4 = glm::vec4(p1 , 0);
	const glm::vec4 p2_vec4 = glm::vec4(p2 , 1);

	const glm::vec4 control1 = p1_vec4 + m_controlPoint1;
	const glm::vec4 control2 = p2_vec4 + m_controlPoint2;

	glm::vec4 top = glm::lerp(control1 , control2 , a);
	glm::vec4 left = glm::lerp(p1_vec4 , control1 , a);
	glm::vec4 right = glm::lerp(control2 , p2_vec4 , a);

	glm::vec4 left_top_lerp  = glm::lerp(left , top , a);
	glm::vec4 top_right_lerp  = glm::lerp(top , right , a);

	glm::vec4 tangent_point = glm::lerp(left_top_lerp , top_right_lerp , a);

	result = glm::vec3(tangent_point);

}
