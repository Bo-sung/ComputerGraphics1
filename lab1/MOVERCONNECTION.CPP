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

AnchoredMoverConnection::AnchoredMoverConnection()
{
	m_anchorTop = cyclone::Vector3(5, 10, 0);
	m_anchorBottom = cyclone::Vector3(5, 0, 0);
	Init(m_anchorTop, m_anchorBottom);
}

AnchoredMoverConnection::AnchoredMoverConnection(cyclone::Vector3 _anchorTop, cyclone::Vector3 _anchorBottom)
{
	Init(_anchorTop, _anchorBottom);
}

void AnchoredMoverConnection::Init(cyclone::Vector3 _anchorTop, cyclone::Vector3 _anchorBottom)
{
	m_anchorTop = _anchorTop;
	m_anchorBottom = _anchorBottom;
	m_forces = new cyclone::ParticleForceRegistry();
	m_gravity = new cyclone::ParticleGravity(cyclone::Vector3::GRAVITY);	// 중력
}

void AnchoredMoverConnection::AddMover(Mover* _mover)
{
	Mover* mover = _mover;
	cyclone::MyAnchoredSpring* spring = new cyclone::MyAnchoredSpring(&m_anchorTop, 5, 3.0);
	//m_forces->add(mover->m_particle, m_gravity);
	m_forces->add(mover->m_particle, spring);
	m_springs.push_back(spring);
	m_movers.push_back(mover);
}

void AnchoredMoverConnection::update(float duration)
{
	m_forces->updateForces(duration);
	for (int i = 0; i < m_movers.size(); i++)
	{
		m_movers[i]->Update(duration);
	}
	for (int i = 0; i < m_springs.size(); i++)
	{
		m_springs[i]->updateForce(m_movers[i]->m_particle, duration);
	}
}

void AnchoredMoverConnection::draw(int shadow)
{
	for (int i = 0; i < m_movers.size(); i++)
	{
		m_movers[i]->draw(shadow);
	}

	if (shadow)
	{
		return;
	}
	glLineWidth(3.0f); //선 굵기
	glBegin(GL_LINES);

	glColor3f(1, 1, 0); //선 색상
	glVertex3f(
		m_anchorTop.x,
		m_anchorTop.y,
		m_anchorTop.z
	);
	glVertex3f(
		m_anchorBottom.x,
		m_anchorBottom.y,
		m_anchorBottom.z
	);

	for (int i = 0; i < m_movers.size(); i++)
	{
		cyclone::Vector3 position;
		m_movers[i]->m_particle->getPosition(&position);
		glColor3f(0, 0, 0); //선 색상
		glVertex3f(
			m_anchorTop.x,
			m_anchorTop.y,
			m_anchorTop.z
		);
		glVertex3f(
			position.x,
			position.y,
			position.z
		);
	}
	glEnd();
	glLineWidth(1.0f);
}
