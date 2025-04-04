#ifndef __MOVER_H
#define __MOVER_H

#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "core.h"
#include "particle.h"

class Mover
{
public:
	// constructors
	Mover(cyclone::Vector3 _position)
	{
		m_position = _position;
		Init();
	};
	Mover(cyclone::Vector3 _position, int _instanceID)
	{
		m_position = _position;
		m_instanceID = _instanceID;
		Init();
	};
	Mover(int _instanceID)
	{
		m_instanceID = _instanceID;
		Init();
	};
	Mover()
	{
		Init();
	};
	~Mover() {};

private:
	struct GLfloat3
	{
		GLfloat r;
		GLfloat g;
		GLfloat b;
	};

	int m_instanceID = -1;
	void Init()
	{
		m_position = cyclone::Vector3(0, 10, 0);
		m_size = 2.0;
		m_particle = new cyclone::Particle();
		m_particle->setPosition(5, 20, 0);	// 초기 위치
		m_particle->setVelocity(0, 0, 0);	// 초기 속도
		m_particle->setMass(1.0f);			// 질량
		m_particle->setDamping(0.9f);		// 댐핑
		m_particle->setAcceleration(cyclone::Vector3::GRAVITY);	// 초기 가속도
		m_meshColor = { 1.0f, 0.0f, 0.0f };	// 색상
		m_shadowColor = { 0.2f, 0.2f, 0.2f };	// 그림자 색상
	}

	void SetColor(GLfloat3 _color)	{ glColor3f(_color.r, _color.g, _color.b); }
	bool CheckFloor()				{ return m_position.y - m_size < 0; }
	bool CheckMapBoundX()			{ return m_position.x > MAX_AREA || m_position.x < -MAX_AREA; }
	bool CheckMapBoundZ()			{ return m_position.z > MAX_AREA || m_position.z < -MAX_AREA; }
	bool CheckMapBound()			{ return CheckMapBoundX() || CheckMapBoundZ(); }
	bool CheckEdges()				{ return CheckFloor() || CheckMapBound(); }

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

public:

	// constants
	const float MAX_AREA = 100.0f;

	// components
	cyclone::Vector3 m_position;
	cyclone::Particle* m_particle;

	// variables
	float m_size;
	GLfloat3 m_meshColor;
	GLfloat3 m_shadowColor;


	void MovePosition(cyclone::Vector3 _position)
	{
		m_position += _position;
	}

	/// <summary>
	/// 업데이트 처리(delta_t)
	/// </summary>
	/// <param name="duration"></param>
	void Update(float _delta_t)
	{
		static cyclone::Vector3 DEFAULT_POSITION = cyclone::Vector3(0, 3, 0);

		m_particle->integrate(_delta_t);
		MakeBounce(CheckEdge());
	}

	void draw(int shadow)
	{
		cyclone::Vector3 position;
		m_particle->getPosition(&position);
		m_position = position;

		if (!shadow)
		{
			// 색상 설정
			SetColor(m_meshColor);

			if (m_instanceID == -1)
			{
				glLoadName(0);
			}
			else
			{
				glLoadName(m_instanceID);
			}
		}
		else
		{
			SetColor(m_shadowColor);
			glColor3f(0.2, 0.2, 0.2);
		}

		// 이동 관련된 처리는 glPushMatrix()와 glPopMatrix() 사이에 처리해야 한다.
		glPushMatrix();
		glTranslatef(m_position.x, m_position.y, m_position.z);
		glutSolidSphere(m_size, 20, 20);
		glPopMatrix();
	}
private:
};

#endif