#include "MoverConnection.h"

using namespace cyclone;

MoverConnection::MoverConnection()
{
	m_particle = new cyclone::Particle();
	m_particle->setVelocity(0, 0, 0);	// 초기 속도
	m_particle->setMass(1.0f);			// 질량
	m_particle->setDamping(1);		// 댐핑
	m_particle->setAcceleration(0, 0, 0);	// 초기 가속도
	m_gravity = new cyclone::ParticleGravity(cyclone::Vector3::GRAVITY);	// 중력
	m_forces = new cyclone::ParticleForceRegistry();
	m_forces->add(m_particle, m_gravity);
	Mover* mover1 = new Mover(10);
	Mover* mover2 = new Mover(11);
	m_movers.push_back(mover1);
	m_movers.push_back(mover2);
	m_spring1 = new MySpring(m_movers[0]->m_particle, 20, 3.0);
	m_spring2 = new MySpring(m_movers[1]->m_particle, 20, 3.0);
	m_forces->add(m_movers[0]->m_particle, m_spring2);
	m_forces->add(m_movers[1]->m_particle, m_spring1);
}

void MoverConnection::update(float duration)
{
	m_forces->updateForces(duration);
	for (int i = 0; i < m_movers.size(); i++)
	{
		m_movers[i]->Update(duration);
	}
}

void MoverConnection::draw(int shadow)
{
	for (int i = 0; i < m_movers.size(); i++)
	{
		m_movers[i]->draw(shadow);
	}
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < m_movers.size(); i++)
	{
		cyclone::Vector3 position;
		m_movers[i]->m_particle->getPosition(&position);
		glVertex3f(position.x, position.y, position.z);
	}

	glEnd();
}

MoverConnection::~MoverConnection()
{
	delete m_particle;
	delete m_gravity;
	delete m_forces;
	delete m_spring1;
	delete m_spring2;
	for (int i = 0; i < m_movers.size(); i++)
	{
		delete m_movers[i];
	}
	m_movers.clear();
}