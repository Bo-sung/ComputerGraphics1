#ifndef __FIREWORK_H
#define __FIREWORK_H

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
#include "Fire.h"
#include "FireworkRule.h"

/// <summary>
/// 阂采愁捞 夯眉
/// </summary>
class Fireworks
{
private:

	/// <summary>
	/// 罚待 float 积己
	/// </summary>
	float RandFloat(float min, float max)
	{
		return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
	}


	/// <summary>
	/// 罚待 氦磐 积己
	/// </summary>
	cyclone::Vector3 MakeRandVec(float _min_x, float _min_y, float _min_z, float _max_x, float _max_y, float _max_z)
	{
		return MakeRandVec(cyclone::Vector3(_min_x, _min_y, _min_z), cyclone::Vector3(_max_x, _max_y, _max_z));
	}

	/// <summary>
	/// 罚待 氦磐 积己
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
		m_rule[0] = new FireworkRule();
		m_rule[1] = new FireworkRule();
		m_rule[2] = new FireworkRule();


		m_rule[0]->setParameters(1, 0.5f, 5.4f, cyclone::Vector3(-5, 10, -5), cyclone::Vector3(5, 15, 5), 0.9f, 20);
		m_rule[1]->setParameters(1, 0.5f, 10.4f, cyclone::Vector3(-15, 5, -5), cyclone::Vector3(15, 6, 15), 0.2f, 40);
		m_rule[2]->setParameters(1, 0.5f, 12.4f, cyclone::Vector3(-5, 0, -5), cyclone::Vector3(5, 5, 5), 0.7f, 25);
	}
public:
	Fireworks()
	{
		Init();
	}
	~Fireworks();

	/// <summary>
	/// 逢
	/// </summary>
	FireworkRule* m_rule[3];
	/// <summary>
	/// Fire 历厘
	/// </summary>
	std::vector<Fire*> m_fireworks;

	/// <summary>
	/// Fire狼 积己/家戈/诀单捞飘
	/// </summary>
	void update(float duration)
	{
		for (int i = 0; i < m_fireworks.size(); ++i)
		{
			bool result = m_fireworks[i]->update(duration);
			// result啊 false 搁 按眉 昏力 棺 氦磐俊辑 昏力
			if (result == false)
			{
				if (m_fireworks[i]->m_type == 0)
				{
					create(m_fireworks[i]);
				}
				delete m_fireworks[i];
				m_fireworks.erase(m_fireworks.begin() + i);
				--i;
			}
		}
	}
	/// <summary>
	/// 弊府扁
	/// </summary>
	void draw(int shadow)
	{
		for (int i = 0; i < m_fireworks.size(); ++i)
		{
			m_fireworks[i]->draw(shadow);
			m_fireworks[i]->drawHistory();
		}
	}
	/// <summary>
	/// Init Fire 积己
	/// </summary>
	void create()
	{
		for (int i = 0; i < 5; ++i)
		{
			Fire* temp = new Fire(0);
			temp->setRule(m_rule[rand() % 3]);
			temp->m_age = RandFloat(0.5f,3.0f);
			m_fireworks.push_back(temp);
		}
	}
	/// <summary>
	/// Child Fire 积己
	/// </summary>
	void create(Fire* parent)
	{
		for (int i = 0; i < 5; ++i)
		{
			Fire* temp = new Fire(1);
			temp->SetPosition(parent->m_position);
			temp->m_particle->setVelocity(parent->m_particle->getVelocity() + MakeRandVec(-5, 10, -5, 5, 15, 5));

			temp->m_age = RandFloat(0.5f, 3.0f);
			m_fireworks.push_back(temp);
		}
	}
};
#endif