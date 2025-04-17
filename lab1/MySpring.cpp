#include "MySpring.h"
#include <iostream>
using namespace cyclone;

MySpring::MySpring(Particle* p, double springConstant, double restLength)
{
	other = p;
	this->springConstant = springConstant;
	this->restLength = restLength;
}

void MySpring::updateForce(Particle* p, real duration)
{
	cyclone::Vector3 force;

	// force = -k(|d| - l) * d
	// d = p1 - p2
	// l = restLength
	// k = springConstant
	// p1 = particle
	// p2 = other
	cyclone::Vector3 pos1, pos2;
	p->getPosition(&pos1);
	other->getPosition(&pos2);
	cyclone::Vector3 d = pos1 - pos2; // d = p1 - p2
	std::cout << "pos1 : " << pos1.toString() << std::endl;
	std::cout << "pos2 : " << pos2.toString() << std::endl;
	std::cout << "d : " << d.toString() << std::endl;
	std::cout << "d.magnitude() : " << d.magnitude() << std::endl;
	std::cout << "restLength : " << restLength << std::endl;
	std::cout << "springConstant : " << springConstant << std::endl;

	// 두 파티클이 같은 위치에 있을 때
	if (d.magnitude() == 0)
		return;

	force = -springConstant * (d.magnitude() - restLength) * d;
	p->addForce(force);
}