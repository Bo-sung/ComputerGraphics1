#ifndef __FIREWORKRULE_H
#define __FIREWORKRULE_H

#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <deque>
#include <cstdlib>
#include <ctime>

#include "core.h"
#include "particle.h"

/// <summary>
/// Fire들의 물리적 속성을 설정하기 위한 클래스 
/// </summary>
class FireworkRule
{
public:
	FireworkRule()
	{
		type = 0;
		minAge = 0.0f;
		maxAge = 0.0f;
		minVelocity = cyclone::Vector3(0, 0, 0);
		maxVelocity = cyclone::Vector3(0, 0, 0);
		damping = 1.0f;
		payloadCount = 1;
	}
	~FireworkRule();

	unsigned type;
	cyclone::real minAge;
	cyclone::real maxAge;
	cyclone::Vector3 minVelocity;
	cyclone::Vector3 maxVelocity;
	cyclone::real damping;
	unsigned payloadCount;

	void setParameters
	(
		unsigned _type,
		cyclone::real _minAge,
		cyclone::real _maxAge,
		cyclone::Vector3& _minVelocity,
		cyclone::Vector3& _maxVelocity,
		cyclone::real _damping,
		int _count
	)
	{
		type = _type;
		minAge = _minAge;
		maxAge = _maxAge;
		minVelocity = _minVelocity;
		maxVelocity = _maxVelocity;
		damping = _damping;
		payloadCount = _count;
	}
};
#endif