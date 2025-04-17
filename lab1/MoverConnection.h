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

class AnchoredMoverConnection
{
public:
	AnchoredMoverConnection();
	AnchoredMoverConnection(cyclone::Vector3 _anchorTop, cyclone::Vector3 _anchorBottom);
	~AnchoredMoverConnection();
	void Init(cyclone::Vector3 _anchorTop, cyclone::Vector3 _anchorBottom);
	cyclone::Vector3 m_anchorTop;
	cyclone::Vector3 m_anchorBottom;
	cyclone::ParticleForceRegistry* m_forces;
	cyclone::ParticleGravity* m_gravity;

	std::vector<Mover*> m_movers;
	std::vector<cyclone::MyAnchoredSpring*> m_springs;

	void AddMover(Mover* mover);
	void update(float duration);
	void draw(int shadow);
};