#ifndef __MOVER_H
#define __MOVER_H

#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>

#include "core.h"
#include "particle.h"
#include "pfgen.h"


class Mover
{
private:

	int m_instanceID = -1;
	void Init()
	{
		m_size = 2.0;
		m_particle = new cyclone::Particle();
		m_particle->setVelocity(0, 0, 0);	// 초기 속도
		m_particle->setMass(5.0f);			// 질량
		m_particle->setDamping(0.9);		// 댐핑
		m_particle->setAcceleration(0, 0, 0);	// 초기 가속도

		m_meshColor = { 1.0f, 0.0f, 0.0f };	// 색상
		m_shadowColor = { 0.2f, 0.2f, 0.2f };	// 그림자 색상
		SetPosition(cyclone::Vector3(0, 10, 0));	// 초기 위치
	}
public:

	cyclone::Quaternion ChangeRotation(cyclone::Vector3 axis, float degree)
	{
		return ChangeRotation(axis, degree, m_position);
	}

	cyclone::Quaternion ChangeRotation(cyclone::Vector3 axis, float degree, cyclone::Vector3 pos)
	{
		cyclone::Quaternion q;
		const float degrees2Radians = 3.141592f / 180;

		q.r = cos(degrees2Radians * degree * 0.5);
		cyclone::Vector3 v = axis * sin(degrees2Radians * degree * 0.5f);
		q.i = v.x;
		q.j = v.y;
		q.k = v.z;

		q.normalise();
		q *= quaternion;
		ChangeOrientationAndPos(q, pos);
		quaternion = q;

		return quaternion;
	}

	void ChangeOrientationAndPos(const cyclone::Quaternion& q, const cyclone::Vector3& pos)
	{
		m_transformMatrix.setOrientationAndPos(q, pos);
	}
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

	cyclone::Matrix4 m_transformMatrix;
	bool m_IsDragging = false;
	cyclone::Vector3 m_LastPickPos;
	clock_t m_LastPickTime;

	void SetColor(GLfloat3 _color) { glColor3f(_color.r, _color.g, _color.b); }
	bool CheckFloor() { return m_position.y - m_size < 0; }
	bool CheckMapBoundX() { return m_position.x > MAX_AREA || m_position.x < -MAX_AREA; }
	bool CheckMapBoundZ() { return m_position.z > MAX_AREA || m_position.z < -MAX_AREA; }
	bool CheckMapBound() { return CheckMapBoundX() || CheckMapBoundZ(); }
	bool CheckEdges() { return CheckFloor() || CheckMapBound(); }

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

		if ((_colAt & BounceType::BOUNCE_FLOOR) == BounceType::BOUNCE_FLOOR)
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
	cyclone::Quaternion quaternion;

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
		std::cout << "time : " << time << " distance : " << distance.toString() << " Velocity : " << v.toString() << std::endl;
		m_particle->setVelocity(v);
	}

	void getGLTransform(float matrix[16])
	{
		matrix[0] = (float)m_transformMatrix.data[0];
		matrix[1] = (float)m_transformMatrix.data[4];
		matrix[2] = (float)m_transformMatrix.data[8];
		matrix[3] = 0;
		matrix[4] = (float)m_transformMatrix.data[1];
		matrix[5] = (float)m_transformMatrix.data[5];
		matrix[6] = (float)m_transformMatrix.data[9];
		matrix[7] = 0;
		matrix[8] = (float)m_transformMatrix.data[2];
		matrix[9] = (float)m_transformMatrix.data[6];
		matrix[10] = (float)m_transformMatrix.data[10];
		matrix[11] = 0;
		matrix[12] = (float)m_transformMatrix.data[3];
		matrix[13] = (float)m_transformMatrix.data[7];
		matrix[14] = (float)m_transformMatrix.data[11];
		matrix[15] = 1;

	}

	void draw(int shadow)
	{
		cyclone::Vector3 position;
		m_particle->getPosition(&position);
		m_position = position;

		GLfloat mat[16];
		getGLTransform(mat); //transformMatrix로 부터 opengl용 행렬로 변경
		if (!shadow) {
			glPushMatrix();
			glMultMatrixf(mat);
			glLineWidth(3.0f);
			glBegin(GL_LINES); //오브젝트에 3개축 그림
			glColor3f(1, 0, 0);
			glVertex3f(0, 0.1, 0);
			glVertex3f(0, 10, 0);
			glColor3f(0, 1, 0);
			glVertex3f(0, 0.1, 0);
			glVertex3f(10, 0.1, 0);
			glColor3f(0, 0, 1);
			glVertex3f(0, 0.1, 0);
			glVertex3f(0, 0.1, 10);
			glEnd();
			glPopMatrix();
			glLineWidth(1.0f);
		}

		if (shadow) { //그림자 구분
			glColor3f(0.2f, 0.2f, 0.2f);
		}
		else {
			glColor3f(1, 0., 0);
		}
		glPushMatrix();
		glMultMatrixf(mat);
		glutSolidCube(3.0f); //박스그리기
		glPopMatrix();
	}
};

#endif