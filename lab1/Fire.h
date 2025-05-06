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
#include <cmath>

#include "core.h"
#include "particle.h"
#include "FireworkRule.h"

struct GLfloat3
{
	GLfloat r;
	GLfloat g;
	GLfloat b;
};
/// <summary>
/// 불꽃
/// </summary>
class Fire
{
protected:

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

	GLfloat3 FadeOutColor(GLfloat3 _color, float _delta_t, float _fadeSpeed = 1.0f)
	{
		_color.r = std::max(0.2f, _color.r - _delta_t * _fadeSpeed);
		_color.g = std::max(0.2f, _color.g - _delta_t * _fadeSpeed);
		_color.b = std::max(0.2f, _color.b - _delta_t * _fadeSpeed);

		return _color;
	}

	void Init(cyclone::Vector3 _SpawnPos, cyclone::Vector3 _Velocity, GLfloat3 _color, int _childType)
	{
		m_size = 0.75f;
		m_particle = new cyclone::Particle();
		m_particle->setVelocity(_Velocity);	// 초기 속도
		m_particle->setMass(1.0f);			// 질량
		m_particle->setDamping(0.99f);		// 댐핑
		m_particle->setAcceleration(cyclone::Vector3::GRAVITY);	// 초기 가속도
		m_meshColor = _color;	// 색상
		m_meshColor_cur = m_meshColor;
		m_traceColor = m_meshColor;
		m_traceColor_cur = m_traceColor;
		m_shadowColor = { 0.3f, 0.3f, 0.3f };	// 그림자 색상
		SetPosition(_SpawnPos);	// 초기 위치
		m_childType = _childType;
	}

	void Init()
	{
		srand((unsigned int)time(NULL) * rand());
		cyclone::Vector3 randVal = MakeRandVec(cyclone::Vector3(0, 0, 0), cyclone::Vector3(10, 30, 10));
		Init
		(
			cyclone::Vector3
			(
				RandFloat(-5, 5),
				10,
				RandFloat(-5, 5)
			),
			randVal,
			{
				RandFloat(0, 1),
				RandFloat(0, 1),
				RandFloat(0, 1)
			},
			0
		);
	}	
public:
	// 0 = init fire, 1 = child fire
	Fire(int type)
	{
		Init();
		m_type = type;
		SetAge(0.0f);
		m_delay = 0.0f;
		m_rule = nullptr;
	}

	Fire(int type, cyclone::Vector3 _SpawnPos, cyclone::Vector3 _Velocity, GLfloat3 _color, int _childType)
	{
		Init(_SpawnPos, _Velocity, _color, _childType);
		m_type = type;
		SetAge(0.0f);
		m_delay = 0.0f;
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
	GLfloat3 m_meshColor_cur;
	GLfloat3 m_shadowColor;
	GLfloat3 m_traceColor;
	GLfloat3 m_traceColor_cur;

	int m_traceLimit = -1;	// history에 출력할 최대 개수

	// components

	/// <summary>
	/// Fire의 타입
	/// </summary>
	int m_type;

	/// <summary>
	/// Fire의 타입
	/// </summary>
	int m_childType;

	/// <summary>
	/// Fire의 현재 age
	/// </summary>
	float m_age;
	float m_age_cur;

	void SetAge(float _age) { m_age = _age; m_age_cur = _age; }

	float m_decayRate = 1;

	/// <summary>
	/// Fire 되기전 딜레이
	/// </summary>
	float m_delay;

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
		if (HandleDelay(_delta_t))
		{
			return true;
		}

		// 동기화
		m_particle->integrate(_delta_t);
		m_particle->getPosition(&m_position);

		movement(_delta_t);


		// 수명 다하거나 포지션이 바닥 아래면 삭제
		return HandleAge(_delta_t) && m_position.y + m_size >= 0;
	}

	void draw(int shadow)
	{
		cyclone::Vector3 position;
		m_particle->getPosition(&position);
		m_position = position;

		if (!shadow)
		{
			glColor3f(m_meshColor_cur.r, m_meshColor_cur.g, m_meshColor_cur.b);
		}
		else
		{
			glColor3f(m_shadowColor.r, m_shadowColor.g, m_shadowColor.b);
		}

		// 이동 관련된 처리는 glPushMatrix()와 glPopMatrix() 사이에 처리해야 한다.
		// 위치 이동, 로테이션, 크기변경과 같은 Transform을 변경하려변 glPushMatrix()와 glPopMatrix() 사이에 작성
		glPushMatrix();
		glTranslatef(m_position.x, m_position.y, m_position.z);
		glutSolidSphere(m_size, 20, 20);
		glPopMatrix();
	}

	void drawHistory(int shadow)
	{
		glLineWidth(2.0f);
		glPushMatrix();

		glBegin(GL_LINE_STRIP);

		if (shadow)
		{
			glColor3f(m_shadowColor.r, m_shadowColor.g, m_shadowColor.b);
		}
		else
		{
			glColor3f(m_traceColor_cur.r, m_traceColor_cur.g, m_traceColor_cur.b);
		}



		if (m_decayRate >= 0.3f)
		{
			int traceLim = m_traceLimit;
			// trace 미설정
			if (traceLim == -1)
				traceLim = m_history.size();
			// m_history에서 역순으로 m_traceLimit 갯수 만큼 그리기. 만약 m_history.size() < m_traceLimit 이면 m_history.size() 만큼 그리기
			for (int i = m_history.size() - 1; i >= 0 && traceLim > 0; --i, --traceLim)
			{
				cyclone::Vector3 pos = m_history[i];
				glVertex3f(pos.x, pos.y, pos.z);
			}
		}

		glEnd();
		glPopMatrix();
		glLineWidth(1.0f);
	}
	bool virtual HandleDelay(float _delta_t)
	{
		m_delay -= _delta_t;
		if (m_delay > 0)
		{
			return true;
		}
		return false;
	}

	bool virtual HandleAge(float _delta_t)
	{
		m_age_cur -= _delta_t;
		if (m_age_cur < 0)
		{
			m_meshColor_cur = FadeOutColor(m_meshColor_cur, _delta_t);
			m_traceColor_cur = FadeOutColor(m_traceColor_cur, _delta_t * 2);
			m_decayRate -= _delta_t;
		}

		return m_decayRate >= 0;
	}

	void virtual movement(float _delta_t)
	{
		if (m_age_cur >= 0)
		{
			m_history.push_back(m_position);	// history에 현재 위치 저장
		}
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
	void SetColor(GLfloat3 _color) { m_meshColor = _color; m_meshColor_cur = m_meshColor;}
	void SetTraceColor(GLfloat3 _color) { m_traceColor = _color; m_traceColor_cur = m_traceColor; glColor3f(_color.r, _color.g, _color.b); }
};

class SpiralFire : public Fire
{
private:
	float _angle = 0.0f;
	float _radius = 0.5f;
	cyclone::Vector3 _explodePos = cyclone::Vector3(0, 0, 0);
public:
	SpiralFire(int type, cyclone::Vector3 _SpawnPos, cyclone::Vector3 _ExpPos, cyclone::Vector3 _Velocity, GLfloat3 _color, int _childType) : Fire(type, _SpawnPos, _Velocity, _color, _childType)
	{
		Init(_SpawnPos,_Velocity,_color, _childType);
		_explodePos = _ExpPos;
		m_type = type;
		SetAge(0.0f);
		m_delay = 0.0f;
		m_rule = nullptr;
	}

	SpiralFire(int type) : Fire(type)
	{
		Init();
		m_type = type;
		SetAge(0.0f);
		m_delay = 0.0f;
		m_rule = nullptr;
	}

	~SpiralFire()
	{
	}

	void movement(float _delta_t) override
	{
		_angle +=  _delta_t * 100;

		// 원운동 성분 (xy 평면 기준)
		cyclone::real x_rot = _radius * cos(_angle);
		cyclone::real y_rot = _radius * sin(_angle);
		cyclone::Vector3 rotationalComponent(x_rot, 0, y_rot);

		SetPosition(cyclone::Vector3(x_rot, m_position.y, y_rot));
		m_history.push_back(m_position);	// history에 현재 위치 저장
	}
};


#endif