#include "MyContact.h"
using namespace cyclone;
MyGroundContact::MyGroundContact()
{
}

MyGroundContact::~MyGroundContact()
{
}

void MyGroundContact::init(cyclone::Particle* p, double size)
{
	if(p)
		m_particles.push_back(p);
	this->m_size = size;
}

unsigned MyGroundContact::addContact(cyclone::ParticleContact* _contact, unsigned _limit) const
{
	unsigned count = 0;
	for (size_t i = 0; i < m_particles.size(); ++i)
	{
		cyclone::Vector3 pos;
		m_particles[i]->getPosition(&pos);
		if (pos.y - m_size < 0)
		{
			_contact->particle[0] = m_particles[i];
			_contact->particle[1] = nullptr;
			_contact->restitution = 0.5f;
			_contact->penetration = m_size - pos.y;
			count++;
			if (count >= _limit)
				return count;
			_contact++;
		}
	}
	return count;
}