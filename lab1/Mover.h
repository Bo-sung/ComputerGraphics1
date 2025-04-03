#ifndef __MOVER_H
#define __MOVER_H

#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "core.h"
#include "particle.h"
#include "PhysicsObject.h"

class Mover
{
public:

	const float MAX_AREA = 100.0f;
	Mover(cyclone::Vector3 _position)
	{
		m_position = _position;
		m_size = 2.0;
		m_particle = new cyclone::Particle();

		m_particle->setPosition(5, 20, 0);	// 초기 위치
		m_particle->setVelocity(0, 0, 0);	// 초기 속도
		m_particle->setMass(1.0f);			// 질량
		m_particle->setDamping(0.9f);		// 댐핑
		m_particle->setAcceleration(cyclone::Vector3::GRAVITY);	// 초기 가속도
	};
	Mover()
	{
		m_position = cyclone::Vector3(0, 10, 0);
		m_size = 2.0;
		m_particle = new cyclone::Particle();

		m_particle->setPosition(5, 20, 0);	// 초기 위치
		m_particle->setVelocity(0, 0, 0);	// 초기 속도
		m_particle->setMass(1.0f);			// 질량
		m_particle->setDamping(0.9f);		// 댐핑
		m_particle->setAcceleration(cyclone::Vector3::GRAVITY);	// 초기 가속도
	};
	~Mover() {};



	cyclone::Vector3 m_position;
	cyclone::Particle* m_particle;

	float m_size;

	void MovePosition(cyclone::Vector3 _position)
	{
		m_position += _position;
	}

	bool CheckFloor()
	{
		if (m_position.y - m_size < 0)
		{
			return true;
		}
		return false;
	}

	bool CheckMapBoundX()
	{
		if (m_position.x > MAX_AREA ||
			m_position.x < -MAX_AREA)
			return true;
		return false;
	}

	bool CheckMapBoundZ()
	{
		if (m_position.z > MAX_AREA ||
			m_position.z < -MAX_AREA)
			return true;
		return false;
	}

	bool CheckMapBound()
	{
		if (CheckMapBoundX() || CheckMapBoundZ())
			return true;
		return false;
	}

	bool CheckEdges()
	{
		if (CheckFloor())
			return true;
		if (CheckMapBound())
			return true;

		return false;
	}

	enum class BounceType
	{
		NONE,
		BOUNCE_FLOOR,
		BOUNCE_MAP_BOUND_X,
		BOUNCE_MAP_BOUND_Z,
	};

	BounceType CheckEdge()
	{
		if (CheckFloor())
			return BounceType::BOUNCE_FLOOR;
		if (CheckMapBoundX())
			return BounceType::BOUNCE_MAP_BOUND_X;
		if (CheckMapBoundZ())
			return BounceType::BOUNCE_MAP_BOUND_Z;

		return BounceType::NONE;
	}

	void MakeBounce()
	{
		cyclone::Vector3 checkposition;
		cyclone::Vector3 vel;
		m_particle->getPosition(&checkposition);
		m_particle->getVelocity(&vel);

		if (CheckFloor())
		{
			checkposition.y = m_size;
			m_particle->setPosition(checkposition);
			vel.y = vel.y * (-1);
			m_particle->setVelocity(vel);
		}

		if (CheckMapBound())
		{
			m_particle->setPosition(cyclone::Vector3(0, checkposition.y, 0));
		}
	}

	void MakeBounce(BounceType _colAt)
	{
		cyclone::Vector3 checkposition;
		cyclone::Vector3 vel;
		m_particle->getPosition(&checkposition);
		m_particle->getVelocity(&vel);

		switch (_colAt)
		{
		case Mover::BounceType::NONE:
			break;
		case Mover::BounceType::BOUNCE_FLOOR:
			checkposition.y = m_size;
			m_particle->setPosition(checkposition);
			vel.y = vel.y * (-1);
			m_particle->setVelocity(vel);
			break;
		case Mover::BounceType::BOUNCE_MAP_BOUND_X:
			checkposition.x -= m_size;
			m_particle->setPosition(checkposition);
			vel.x = vel.x * (-1);
			m_particle->setVelocity(vel);
			break;
		case Mover::BounceType::BOUNCE_MAP_BOUND_Z:
			checkposition.z -= m_size;
			m_particle->setPosition(checkposition);
			vel.z = vel.z * (-1);
			m_particle->setVelocity(vel);
			break;
		default:
			break;
		}
	}


	/// <summary>
	/// 적분하는 함수(delta_t)
	/// </summary>
	/// <param name="duration"></param>
	void Update(float _delta_t)
	{
		static cyclone::Vector3 DEFAULT_POSITION = cyclone::Vector3(0, 3, 0);

		m_particle->integrate(_delta_t);
		//m_particle->setPosition(m_particle->getPosition() + cyclone::Vector3(0.5, 0, 0));
		//m_particle->addForce(cyclone::Vector3(1,0,0));
		if (CheckEdges())
		{
			MakeBounce();
		}
	}


	void draw(int shadow)
	{
		cyclone::Vector3 position;
		m_particle->getPosition(&position);
		m_position = position;
		if (shadow)
		{
			glColor3f(0.2, 0.2, 0.2);
		}
		else
		{
			glColor3f(1, 0, 0);
		}

		// 위치 이동, 로테이션, 크기변경과 같은 Transform을 변경하려변 glPushMatrix()와 glPopMatrix() 사이에 작성
		glPushMatrix();
		glTranslatef(m_position.x, m_position.y, m_position.z);
		glutSolidSphere(m_size, 20, 20);
		glPopMatrix();
	}
private:
};

#endif