#ifndef __MOVER_H
#define __MOVER_H

#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>

#include "core.h"
#include "particle.h"


class Mover
{
private:

	int m_instanceID = -1;
	void Init()
	{
		m_size = 2.0;
		m_particle = new cyclone::Particle();
		m_particle->setVelocity(0, 0, 0);	// 초기 속도
		m_particle->setMass(1.0f);			// 질량
		m_particle->setDamping(0.9f);		// 댐핑
		m_particle->setAcceleration(cyclone::Vector3::GRAVITY);	// 초기 가속도
		m_meshColor = { 1.0f, 0.0f, 0.0f };	// 색상
		m_shadowColor = { 0.2f, 0.2f, 0.2f };	// 그림자 색상
		SetPosition(cyclone::Vector3(0, 10, 0));	// 초기 위치
	}
public:
	// constructors
	Mover(cyclone::Vector3 _position)
	{
		Init();
		SetPosition(_position);
	};
	Mover(cyclone::Vector3 _position, int _instanceID)
	{
		m_instanceID = _instanceID;
		Init();
		SetPosition(_position);
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

	int GetInstanceID() { return m_instanceID; }

private:
	struct GLfloat3
	{
		GLfloat r;
		GLfloat g;
		GLfloat b;
	};

	bool m_IsDragging = false;
	cyclone::Vector3 m_LastPickPos;
	clock_t m_LastPickTime;

	void SetColor(GLfloat3 _color)	{ glColor3f(_color.r, _color.g, _color.b); }
	bool CheckFloor()				{ return m_position.y - m_size < 0; }
	bool CheckMapBoundX()			{ return m_position.x > MAX_AREA || m_position.x < -MAX_AREA; }
	bool CheckMapBoundZ()			{ return m_position.z > MAX_AREA || m_position.z < -MAX_AREA; }
	bool CheckMapBound()			{ return CheckMapBoundX() || CheckMapBoundZ(); }
	bool CheckEdges()				{ return CheckFloor() || CheckMapBound(); }

	enum BounceType
	{
		NONE = (0x01 << 0),
		BOUNCE_FLOOR = (0x01 << 1),
		BOUNCE_MAP_BOUND_X = (0x01 << 2),
		BOUNCE_MAP_BOUND_Z = (0x01 << 3)
	};
	// CheckEdge()의 결과를 비트마스크로 사용하기 위한 enum
	BounceType CheckEdge()
	{
		BounceType result = NONE; // 초기화

		if (CheckFloor())
			result = static_cast<BounceType>(result | BOUNCE_FLOOR);
		if (CheckMapBoundX())
			result = static_cast<BounceType>(result | BOUNCE_MAP_BOUND_X);
		if (CheckMapBoundZ())
			result = static_cast<BounceType>(result | BOUNCE_MAP_BOUND_Z);

		return result;
	}

	void MakeBounce(BounceType _colAt)
	{
		cyclone::Vector3 checkposition;
		cyclone::Vector3 vel;
		m_particle->getPosition(&checkposition);
		m_particle->getVelocity(&vel);

		if ((_colAt&BounceType::BOUNCE_FLOOR) == BounceType::BOUNCE_FLOOR)
		{
			checkposition.y = m_size;
			m_particle->setPosition(checkposition);
			vel.y = vel.y * (-1);
			m_particle->setVelocity(vel);
		}

		if ((_colAt & BounceType::BOUNCE_MAP_BOUND_X) == BounceType::BOUNCE_MAP_BOUND_X)
		{
			checkposition.x -= checkposition.x > 0 ? m_size : -m_size;
			m_particle->setPosition(checkposition);
			vel.x = vel.x * (-1);
			m_particle->setVelocity(vel);
		}

		if ((_colAt & BounceType::BOUNCE_MAP_BOUND_Z) == BounceType::BOUNCE_MAP_BOUND_Z)
		{
			checkposition.z -= checkposition.z > 0 ? m_size : -m_size;
			m_particle->setPosition(checkposition);
			vel.z = vel.z * (-1);
			m_particle->setVelocity(vel);
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


	// 포지션 값은 무조건 여기서
	void SetPosition(cyclone::Vector3 _position)
	{
		m_position = _position;
		m_particle->setPosition(m_position);
	}

	void MovePosition(cyclone::Vector3 _position)
	{
		SetPosition(m_position + _position);
	}

	/// <summary>
	/// 업데이트 처리(delta_t)
	/// </summary>
	/// <param name="duration"></param>
	void Update(float _delta_t)
	{
		static cyclone::Vector3 DEFAULT_POSITION = cyclone::Vector3(0, 3, 0);

		m_particle->integrate(_delta_t);
		m_particle->getPosition(&m_position);
		if (!m_IsDragging)
			MakeBounce(CheckEdge());

	}

	void OnPick()
	{
		draw(0);
	}

	void OnDrag(cyclone::Vector3 _mousePos)
	{
		if (!m_IsDragging)
		{
			m_IsDragging = true;
			SetPosition(_mousePos);
			m_LastPickPos = m_position;
			m_LastPickTime = clock();
		}
		SetPosition(cyclone::Vector3(_mousePos.x, m_position.y, _mousePos.z));
		m_particle->setVelocity(cyclone::Vector3(0, 0, 0));
		m_particle->setAcceleration(cyclone::Vector3(0, 0, 0));
	}

	void OnRelease(cyclone::Vector3 _mousePos)
	{
		m_IsDragging = false;
		m_particle->setAcceleration(cyclone::Vector3::GRAVITY);
		cyclone::Vector3 curPickPos = _mousePos;
		clock_t curPickTime = clock();

		cyclone::Vector3 distance = curPickPos - m_LastPickPos;
		clock_t time = curPickTime - m_LastPickTime;
		time = time / CLOCKS_PER_SEC;
		if (time == 0)
			time = 1;
		cyclone::Vector3 v = distance / time;
		std::cout << "time : " << time <<  " distance : " << distance.toString() << " Velocity : " << v.toString() << std::endl;
		m_particle->setVelocity(v);
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
		}

		// 이동 관련된 처리는 glPushMatrix()와 glPopMatrix() 사이에 처리해야 한다.
		// 위치 이동, 로테이션, 크기변경과 같은 Transform을 변경하려변 glPushMatrix()와 glPopMatrix() 사이에 작성
		glPushMatrix();
		glTranslatef(m_position.x, m_position.y, m_position.z);
		glutSolidSphere(m_size, 20, 20);
		glPopMatrix();
	}
};

#endif