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
		m_particle->setMass(1.5f);			// 질량
		m_particle->setDamping(0.9f);		// 댐핑
		m_particle->setAcceleration(cyclone::Vector3::GRAVITY);	// 초기 가속도
	};
	~Mover() {};



	cyclone::Vector3 m_position;
	cyclone::Particle* m_particle;
	float m_size;

	cyclone::Vector3 PlaneEdge1 = cyclone::Vector3(20., 0., -25.);	// A
	cyclone::Vector3 PlaneEdge2 = cyclone::Vector3(20., 0., 25.);	// B
	cyclone::Vector3 PlaneEdge3 = cyclone::Vector3(-20., 30., 25.);	// C
	cyclone::Vector3 PlaneEdge4 = cyclone::Vector3(-20., 30., -25.);	// D

	void MovePosition(cyclone::Vector3 _position)
	{
		m_position += _position;
	}

	// 면 법선 벡터
	cyclone::Vector3 GetPlaneNormal()
	{
		static cyclone::Vector3 result = cyclone::Vector3(0, 0, 0);

		if(result != cyclone::Vector3(0, 0, 0))
			return result;

		//P₁P₂ = (x₂-x₁, y₂-y₁, z₂-z₁)
		//P₁P₃ = (x₃-x₁, y₃-y₁, z₃-z₁)
		cyclone::Vector3 p2p1 = PlaneEdge2 - PlaneEdge1;
		cyclone::Vector3 p3p1 = PlaneEdge3 - PlaneEdge1;
		//n = P₁P₂ × P₁P₃법선 백터
		result = p3p1.cross(p2p1);
		return result;
	}

	float GetPlaneConstant()
	{
		static float d = 0.0f;

		if (d != 0.0f)
			return d;

		cyclone::Vector3 n = GetPlaneNormal();
		//d = -(ax₁ + by₁ + cz₁)
		d = -(n.x * PlaneEdge1.x + n.y * PlaneEdge1.y + n.z * PlaneEdge1.z);

		return d;
	}

	bool CheckPlane()
	{
		cyclone::Vector3 n = GetPlaneNormal();
		//d = -(ax₁ + by₁ + cz₁)
		float d = GetPlaneConstant();
		//거리 = |ax₀ + by₀ + cz₀ + d| / √(a² + b² + c²)
		float distance = abs(n.x * m_position.x + n.y * m_position.y + n.z * m_position.z + d) / sqrt(n.x * n.x + n.y * n.y + n.z * n.z);

		printf("%f \n", distance);
		if (distance < m_size)
		{
			return true;
		}

		return false;
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
		if (CheckPlane())
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

		if (CheckPlane())
		{
			cyclone::Vector3 n = GetPlaneNormal();
			float d = GetPlaneConstant();

			float distance = abs(n.x * m_position.x + n.y * m_position.y + n.z * m_position.z + d) / sqrt(n.x * n.x + n.y * n.y + n.z * n.z);

			n.normalise();

			distance = m_size - distance;
			//v' = v - 2(v·n)n

			vel = vel - (2 * (vel * n) * n);
			m_particle->setVelocity(vel);

			m_particle->setPosition(checkposition + n * distance);
		}

		//if (CheckFloor())
		//{
		//	checkposition.y = m_size;
		//	m_particle->setPosition(checkposition);
		//	vel.y = vel.y * (-1);
		//	m_particle->setVelocity(vel);
		//}

		BounceType flag = CheckEdge();

		if (flag == BounceType::NONE)
			return;

		if (flag == BounceType::BOUNCE_FLOOR)
		{
			checkposition.y = m_size;
			m_particle->setPosition(checkposition);
			vel.y = vel.y * (-1);
			m_particle->setVelocity(vel);
		}

		if (flag == BounceType::BOUNCE_MAP_BOUND_X)
		{
			checkposition.x -= m_size;
			m_particle->setPosition(checkposition);
			vel.x = vel.x * (-1);
			m_particle->setVelocity(vel);
		}

		if (flag == BounceType::BOUNCE_MAP_BOUND_Z)
		{
			checkposition.z -= m_size;
			m_particle->setPosition(checkposition);
			vel.z = vel.z * (-1);
			m_particle->setVelocity(vel);
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