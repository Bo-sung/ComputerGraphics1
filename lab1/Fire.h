#ifndef __FIRE_H
#define __FIRE_H

#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <deque>
#include <cstdlib>
#include <ctime>

#include "core.h"
#include "particle.h"
#include "FireworkRule.h"

/// <summary>
/// 불꽃
/// </summary>
class Fire
{
private:
	struct GLfloat3
	{
		GLfloat r;
		GLfloat g;
		GLfloat b;
	};

	/// <summary>
	/// 랜덤 float 생성
	/// </summary>
	float RandFloat(float min, float max)
	{
		return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
	}

	/// <summary>
	/// 랜덤 벡터 생성
	/// </summary>
	cyclone::Vector3 MakeRandVec(float _min_x, float _min_y, float _min_z, float _max_x, float _max_y, float _max_z)
	{
		return MakeRandVec(cyclone::Vector3(_min_x, _min_y, _min_z), cyclone::Vector3(_max_x, _max_y, _max_z));
	}

	/// <summary>
	/// 랜덤 벡터 생성
	/// </summary>
	cyclone::Vector3 MakeRandVec(cyclone::Vector3 _min, cyclone::Vector3 _max)
	{
		return cyclone::Vector3
		(
			RandFloat(_min.x, _max.x),
			RandFloat(_min.y, _max.y),
			RandFloat(_min.z, _max.z)
		);
	}

	void Init()
	{
		srand((unsigned int)time(NULL) * rand());
		m_size = 1.0;
		m_particle = new cyclone::Particle();
		cyclone::Vector3 randVal = MakeRandVec(cyclone::Vector3(0, 0, 0), cyclone::Vector3(10, 30, 10));
		m_particle->setVelocity(randVal);	// 초기 속도
		m_particle->setMass(1.0f);			// 질량
		m_particle->setDamping(0.99f);		// 댐핑
		m_particle->setAcceleration(cyclone::Vector3::GRAVITY);	// 초기 가속도
		m_meshColor =
		{
			RandFloat(0, 1),
			RandFloat(0, 1),
			RandFloat(0, 1)
		};	// 색상
		m_shadowColor = { 0.2f, 0.2f, 0.2f };	// 그림자 색상
		SetPosition
		(
			cyclone::Vector3
			(
				RandFloat(-5,5),
				10,
				RandFloat(-5,5)
			)
		);	// 초기 위치

	}	
public:
	// 0 = init fire, 1 = child fire
	Fire(int type)
	{
		Init();
		m_type = type;
		m_age = 0.0f;
		m_rule = nullptr;
	}
	~Fire()
	{

	}

	/// <summary>
	/// 그리기 위한 Fire의 크기
	/// </summary>
	float m_size;
	GLfloat3 m_meshColor;
	GLfloat3 m_shadowColor;

	// components

	/// <summary>
	/// Fire의 타입
	/// </summary>
	int m_type;
	/// <summary>
	/// Fire의 현재 age
	/// </summary>
	float m_age;
	/// <summary>
	/// 파티클 객체
	/// </summary>
	cyclone::Particle* m_particle;

	/// <summary>
	/// 현재 포지션
	/// </summary>
	cyclone::Vector3 m_position;

	/// <summary>
	/// 위치 history 저장
	/// </summary>
	std::deque<cyclone::Vector3> m_history;

	/// <summary>
	/// 적용중인 룰
	/// </summary>
	FireworkRule* m_rule;

	bool update(float _delta_t)
	{
		m_particle->integrate(_delta_t);
		m_particle->getPosition(&m_position);
		m_history.push_back(m_position);	// history에 현재 위치 저장
		m_age -= _delta_t;

		// 수명 다하거나 포지션이 바닥 아래면 삭제
		return m_age >= 0 && m_position.y + m_size >= 0;
	}

	void draw(int shadow)
	{
		cyclone::Vector3 position;
		m_particle->getPosition(&position);
		m_position = position;

		if (!shadow)
		{
			SetColor(m_meshColor);
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

	void drawHistory()
	{
		glLineWidth(2.0f);
		glPushMatrix();

		glBegin(GL_LINE_STRIP);
		for (unsigned int i = 0; i < m_history.size(); i += 2)
		{
			cyclone::Vector3 pos = m_history[i];
			glVertex3f(pos.x, pos.y, pos.z);
		}

		glEnd();
		glPopMatrix();
		glLineWidth(1.0f);
	}
	/// <summary>
	/// 특정 룰을 설정
	/// </summary>
	void setRule(FireworkRule* rule)
	{
		m_rule = rule;
	}
	/// <summary>
	/// 현재 위치를 m_history에 저장
	/// </summary>
	void putHistory()
	{
		m_history.push_back(m_position);
	}

	// 포지션 값은 무조건 여기서
	void SetPosition(cyclone::Vector3 _position)
	{
		m_position = _position;
		m_particle->setPosition(m_position);
	}

	void MovePosition(cyclone::Vector3 _position) { SetPosition(m_position + _position); }
	void SetColor(GLfloat3 _color) { glColor3f(_color.r, _color.g, _color.b); }
};


#endif