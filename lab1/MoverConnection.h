#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>

#include "core.h"
#include "particle.h"
#include "pfgen.h"
#include "Mover.h"
#include "MySpring.h"

class MoverConnection
{
public:
	MoverConnection();
	~MoverConnection();
	cyclone::Particle* m_particle;
	cyclone::ParticleForceRegistry* m_forces;
	cyclone::ParticleGravity* m_gravity;
	cyclone::MySpring* m_spring1;
	cyclone::MySpring* m_spring2;
	std::vector<Mover*> m_movers;
	void update(float duration);
	void draw(int shadow);
};