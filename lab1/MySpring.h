#ifndef __MY_SPRING_H
#define __MY_SPRING_H

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <core.h>
#include "particle.h"

#include <pfgen.h>

namespace cyclone {
	class MySpring : public cyclone::ParticleForceGenerator
	{
		cyclone::Particle* other;
		double springConstant;
		double restLength;
	public:
		MySpring(cyclone::Particle* p, double springConstant, double restLength);
		virtual void updateForce(cyclone::Particle* p, double duration);
	};
}
#endif 