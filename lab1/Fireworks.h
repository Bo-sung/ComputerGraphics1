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
/// 불꽃놀이 본체
/// </summary>
class Fireworks
{
private:

	const int FINAL = 0;							// 최종
	const int SPHERE = 2;							// 구형
	const int SPHERE_TRACE = 21;					// 구형
	const int CIRCLE = 3;							// 원형
	const int CIRCLE_TRACE = 31;					// 원형
	const int DOUBLE_CIRCLE = 32;					// 원형
	const int DOUBLE_CIRCLE_TRACE = 33;				// 원형
	const int NONE = 10;;							// 기본
	const int NONE_TRACE = 11;;						// 기본
	const int FINALE = 100;							// 피날레


	/// <summary>
	/// 랜덤 int 생성
	/// </summary>
	int RandInt(int min, int max)
	{
		return min + static_cast<int>(rand()) / (static_cast<int>(RAND_MAX / (max - min)));
	}

	/// <summary>
	/// 랜덤 float 생성
	/// </summary>
	float RandFloat(float min, float max)
	{
		return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
	}

	/// <summary>
	/// 랜덤 double 생성
	/// </summary>
	double RandDouble(double min, double max)
	{
		return min + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / (max - min)));
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
		m_rule[0] = new FireworkRule();
		m_rule[1] = new FireworkRule();
		m_rule[2] = new FireworkRule();


		m_rule[0]->setParameters(1, 0.5f, 5.4f, cyclone::Vector3(-5, 10, -5), cyclone::Vector3(5, 15, 5), 0.9f, 20);
		m_rule[1]->setParameters(1, 0.5f, 10.4f, cyclone::Vector3(-15, 5, -5), cyclone::Vector3(15, 6, 15), 0.2f, 40);
		m_rule[2]->setParameters(1, 0.5f, 12.4f, cyclone::Vector3(-5, 0, -5), cyclone::Vector3(5, 5, 5), 0.7f, 25);
	}

	// 로드리게스 회전 공식을 이용한 벡터 회전 함수
	cyclone::Vector3 rotate_vector_rodrigues(cyclone::Vector3 axis, cyclone::Vector3 vector, double angle_radians)
	{
		cyclone::Vector3 normalized_axis = axis;
		normalized_axis.normalise();

		double cos_theta = std::cos(angle_radians);
		double sin_theta = std::sin(angle_radians);
		cyclone::Vector3 cross_av = normalized_axis.cross(vector);
		double dot_av = normalized_axis.dot(vector);

		cyclone::Vector3 rotated_vector;
		rotated_vector.x = vector.x * cos_theta + cross_av.x * sin_theta + normalized_axis.x * dot_av * (1.0 - cos_theta);
		rotated_vector.y = vector.y * cos_theta + cross_av.y * sin_theta + normalized_axis.y * dot_av * (1.0 - cos_theta);
		rotated_vector.z = vector.z * cos_theta + cross_av.z * sin_theta + normalized_axis.z * dot_av * (1.0 - cos_theta);

		return rotated_vector;
	}

	/// <summary>
	/// 중점에서 rotation 벡터를 법선으로 사방으로 퍼져나가는 방향벡터 생성.
	/// </summary>
	std::vector<cyclone::Vector3> GeneratePolygonVertics(int sides, cyclone::Vector3 rotation)
	{
		constexpr double PI = 3.14159265358979323846;
		std::vector<cyclone::Vector3> vertices;

		if (sides < 3)
		{
			std::cerr << "정다각형은 최소 3개의 꼭지점이 필요합니다." << std::endl;
			return vertices;
		}
		// 법선 벡터 생성
		cyclone::Vector3 arbitrary_vector;
		if (fabs(rotation.x) > 1e-6)
		{
			arbitrary_vector = cyclone::Vector3(0, 1, 0);
		}
		else if (fabs(rotation.y) > 1e-6)
		{
			arbitrary_vector = cyclone::Vector3(1, 0, 0);
		}
		else if (fabs(rotation.z) > 1e-6)
		{
			arbitrary_vector = cyclone::Vector3(0, 1, 0);
		}


		cyclone::Vector3 perpendicular_vector = rotation.cross(arbitrary_vector);
		perpendicular_vector.normalise();
		for (int i = 0; i < sides; ++i)
		{
			double theta = (2 * PI * i) / sides;
			// 법선 벡터 방향으로 회전 변환 적용
			cyclone::Vector3 vertex = rotate_vector_rodrigues(rotation, perpendicular_vector, theta);
			vertex.normalise();

			vertices.emplace_back(vertex);
		}

		return vertices;
	}

	std::vector<cyclone::Vector3> GenerateSphereVertics(int sides, cyclone::Vector3 rotation)
	{
		std::vector<cyclone::Vector3> temp = GeneratePolygonVertics(sides, rotation);
		std::vector<cyclone::Vector3> vertices;
		for (size_t i = 0; i < temp.size(); i++)
		{
			std::vector<cyclone::Vector3> temp2 = GeneratePolygonVertics(sides, temp[i]);
			vertices.insert(vertices.end(), temp2.begin(), temp2.end());
		}

		return vertices;
	}

	void CreateRadialBurst(int type, int childType, Fire* parent, float age, float delay, int vertics, double radius, cyclone::Vector3 rotation)
	{
		CreateRadialBurst(type, childType, parent, age, delay, vertics, radius, rotation, parent->m_meshColor);
	}
	void CreateRadialBurst(int type, int childType, Fire* parent, float age, float delay, int vertics, double radius, cyclone::Vector3 rotation, GLfloat3 color, int traceLimit = -1)
	{
		CreateRadialBurst(type, childType, parent->m_position, age, delay, vertics, radius, rotation, color, traceLimit);
	}
	void CreateRadialBurst(int type, int childType, cyclone::Vector3 center, float age, float delay, int vertics, double radius)
	{
		CreateRadialBurst(type, childType, center, age, delay, vertics, radius, cyclone::Vector3(0, 1, 0), { 1,1,1 });
	}
	void CreateRadialBurst(int type, int childType, cyclone::Vector3 center, float age, float delay, int vertics, double radius, cyclone::Vector3 rotation)
	{
		CreateRadialBurst(type, childType, center, age, delay, vertics, radius, rotation, { 1,1,1 });
	}
	void CreateRadialBurst(int type, int childType, cyclone::Vector3 center, float age, float delay, int vertics, double radius, cyclone::Vector3 rotation, GLfloat3 color, int traceLimit = -1)
	{
		rotation.normalise();
		std::vector<cyclone::Vector3> positions = GeneratePolygonVertics(vertics, rotation);

		for (int i = 0; i < vertics; ++i)
		{
			Fire* temp = new Fire(type, center, (positions[i] + cyclone::Vector3(0, 1, 0)) * radius, color, childType);
			temp->SetAge(age);
			temp->m_delay = delay;
			temp->m_traceLimit = traceLimit;
			m_fireworks.push_back(temp);
		}
	}
	void CreateSphereBurst(int type, int childType, Fire* parent, float age, float delay, int vertics, double radius)
	{
		CreateSphereBurst(type, childType, parent->m_position, age, delay, vertics, radius, cyclone::Vector3(0, 1, 0), { 1,1,1 });
	}
	void CreateSphereBurst(int type, int childType, Fire* parent, float age, float delay, int vertics, double radius, cyclone::Vector3 rotation)
	{
		CreateSphereBurst(type, childType, parent->m_position, age, delay, vertics, radius, rotation, { 1,1,1 });
	}
	void CreateSphereBurst(int type, int childType, Fire* parent, float age, float delay, int vertics, double radius, cyclone::Vector3 rotation, GLfloat3 color, int traceLimit = -1)
	{
		CreateSphereBurst(type, childType, parent->m_position, age, delay, vertics, radius, rotation, color, traceLimit);
	}
	void CreateSphereBurst(int type, int childType, cyclone::Vector3 center, float age, float delay, int vertics, double radius)
	{
		CreateSphereBurst(type, childType, center, age, delay, vertics, radius, cyclone::Vector3(0, 1, 0), { 1,1,1 });
	}
	void CreateSphereBurst(int type, int childType, cyclone::Vector3 center, float age, float delay, int vertics, double radius, cyclone::Vector3 rotation)
	{
		CreateSphereBurst(type, childType, center, age, delay, vertics, radius, rotation, { 1,1,1 });
	}
	void CreateSphereBurst(int type, int childType, cyclone::Vector3 center, float age, float delay, int vertics, double radius, cyclone::Vector3 rotation, GLfloat3 color, int traceLimit = -1)
	{
		rotation.normalise();
		std::vector<cyclone::Vector3> positions = GenerateSphereVertics(vertics, rotation);

		for (int i = 0; i < positions.size(); ++i)
		{
			Fire* temp = new Fire(type, center, (positions[i] + cyclone::Vector3(0, 1, 0)) * radius, color, childType);
			temp->SetAge(age);
			temp->m_delay = delay;
			temp->m_traceLimit = traceLimit;
			m_fireworks.push_back(temp);
		}
	}


public:
	Fireworks()
	{
		Init();
	}
	~Fireworks();

	/// <summary>
	/// 룰
	/// </summary>
	FireworkRule* m_rule[3];
	/// <summary>
	/// Fire 저장
	/// </summary>
	std::vector<Fire*> m_fireworks;

	/// <summary>
	/// Fire의 생성/소멸/업데이트
	/// </summary>
	void update(float duration)
	{
		for (int i = 0; i < m_fireworks.size(); ++i)
		{
			bool result = m_fireworks[i]->update(duration);
			// result가 false 면 객체 삭제 및 벡터에서 삭제
			if (result == false)
			{
				create(m_fireworks[i]);
				delete m_fireworks[i];
				m_fireworks.erase(m_fireworks.begin() + i);
				--i;
			}
		}
	}
	/// <summary>
	/// 그리기
	/// </summary>
	void draw(int shadow)
	{
		for (int i = 0; i < m_fireworks.size(); ++i)
		{
			m_fireworks[i]->draw(shadow);
			m_fireworks[i]->drawHistory(shadow);
		}
	}
	/// <summary>
	/// Init Fire 생성
	/// </summary>
	void create()
	{
		int count = 20;
		double radius = 70;
		float lastDelay = 0;
		std::vector<cyclone::Vector3> linePositions = {
			cyclone::Vector3(0, 0, -20),
			cyclone::Vector3(0, 0, -15),
			cyclone::Vector3(0, 0, -10),
			cyclone::Vector3(0, 0, -5),
			cyclone::Vector3(0, 0, 0),
			cyclone::Vector3(0, 0, 5),
			cyclone::Vector3(0, 0, 10),
			cyclone::Vector3(0, 0, 15),
			cyclone::Vector3(0, 0, 20),
			cyclone::Vector3(20, 0, 0),
			cyclone::Vector3(15, 0, 0),
			cyclone::Vector3(10, 0, 0),
			cyclone::Vector3(5, 0, 0),
			cyclone::Vector3(0, 0, 0),
			cyclone::Vector3(-5, 0, 0),
			cyclone::Vector3(-10, 0, 0),
			cyclone::Vector3(-15, 0, 0),
			cyclone::Vector3(-20, 0, 0),
		};
		for (int j = 0; j < linePositions.size(); ++j)
		{
			Fire* fire1 = new Fire(CIRCLE, linePositions[j], cyclone::Vector3(0, 50, 0), { 1,1,0.2f }, FINAL);
			fire1->SetAge(3.0f);
			fire1->m_delay = lastDelay;
			fire1->m_traceLimit = 15;
			m_fireworks.push_back(fire1);
			lastDelay += 0.5f;
		}


		cyclone::Vector3 randVelocity = MakeRandVec(cyclone::Vector3(-10, 0, -10), cyclone::Vector3(10, 30, 10));
		cyclone::Vector3 center = cyclone::Vector3(0, 0, 0);
		cyclone::Vector3 rotation = cyclone::Vector3(0, 1, 0);
		rotation.normalise();
		std::vector<cyclone::Vector3> positions = GeneratePolygonVertics(count, rotation);
		for (int i = 0; i < count / 2; ++i)
		{

			Fire* fire1 = new Fire(SPHERE, (positions[i]) * radius + center, cyclone::Vector3(0, 50, 0), { 1,1,0.2f }, FINAL);
			fire1->SetAge(3.0f);
			fire1->m_delay = lastDelay;
			fire1->m_traceLimit = 15;
			m_fireworks.push_back(fire1);
			// 반대편
			Fire* fire2 = new Fire(SPHERE, (positions[i + (count / 2)]) * radius + center, cyclone::Vector3(0, 50, 0), { 1,1,0.2f }, FINAL);
			fire2->SetAge(3.0f);
			fire2->m_delay = lastDelay;
			fire2->m_traceLimit = 15;
			m_fireworks.push_back(fire2);
			lastDelay += 1.25f;
		}
		lastDelay += 3.0f;
		SpiralFire* spiralFire = new SpiralFire(FINALE, cyclone::Vector3(0, 0, 0), cyclone::Vector3(0, 50, 0), cyclone::Vector3(0, 50, 0), { 1,1,0.2f }, FINAL);
		spiralFire->SetAge(3.0f);
		spiralFire->m_delay = lastDelay;
		m_fireworks.push_back(spiralFire);
	}

	/// <summary>
	/// Child Fire 생성
	/// </summary>
	void create(Fire* parent)
	{
		if (parent->m_type == FINAL)
		{
			return;
		}
		else if (parent->m_type == NONE)
		{
			Fire* temp = new Fire(FINAL, parent->m_position, parent->m_particle->getVelocity(), { RandFloat(0, 1),RandFloat(0, 1),RandFloat(0, 1) }, FINAL);
			temp->SetAge(2.5f);
			m_fireworks.push_back(temp);
		}
		else if (parent->m_type == NONE_TRACE)
		{
			Fire* temp = new Fire(FINAL, parent->m_position, parent->m_particle->getVelocity(), { RandFloat(0, 1),RandFloat(0, 1),RandFloat(0, 1) }, FINAL);
			temp->SetAge(2.5f);
			temp->m_traceLimit = 0;
			m_fireworks.push_back(temp);
		}
		else if (parent->m_type == SPHERE)
		{
			auto rotate = cyclone::Vector3(RandFloat(-1.0f, 1.0f), RandFloat(-1.0f, 1.0f), RandFloat(-1.0f, 1.0f));
			rotate.normalise();
			CreateSphereBurst(parent->m_childType, 0, parent, 2.5f, 0, RandInt(10, 20), RandDouble(7, 30), rotate, { RandFloat(0, 1),RandFloat(0, 1),RandFloat(0, 1) }, 0);
		}
		else if (parent->m_type == SPHERE_TRACE)
		{
			auto rotate = cyclone::Vector3(RandFloat(-1.0f, 1.0f), RandFloat(-1.0f, 1.0f), RandFloat(-1.0f, 1.0f));
			rotate.normalise();
			CreateSphereBurst(parent->m_childType, 0, parent, 2.5f, 0, RandInt(10, 20), RandDouble(7, 30), rotate, { RandFloat(0, 1),RandFloat(0, 1),RandFloat(0, 1) });
		}
		else if (parent->m_type == CIRCLE)
		{
			auto rotate = cyclone::Vector3(RandFloat(-1.0f, 1.0f), RandFloat(-1.0f, 1.0f), RandFloat(-1.0f, 1.0f));
			rotate.normalise();
			CreateRadialBurst(parent->m_childType, 0, parent, 2.5f, 0, 20, 25, rotate, { RandFloat(0, 1),RandFloat(0, 1),RandFloat(0, 1) }, 0);
		}
		else if (parent->m_type == CIRCLE_TRACE)
		{
			auto rotate = cyclone::Vector3(RandFloat(-1.0f, 1.0f), RandFloat(-1.0f, 1.0f), RandFloat(-1.0f, 1.0f));
			rotate.normalise();
			CreateRadialBurst(parent->m_childType, 0, parent, 2.5f, 0, 20, 25, rotate, { RandFloat(0, 1),RandFloat(0, 1),RandFloat(0, 1) });
		}
		else if (parent->m_type == DOUBLE_CIRCLE)
		{
			auto rotate = cyclone::Vector3(0, 1, 0);
			int vert = 30;
			double radius = RandDouble(5, 30);
			CreateRadialBurst(parent->m_childType, 0, parent, 2.5f, 0, vert, radius, rotate, { RandFloat(0, 1),RandFloat(0, 1),RandFloat(0, 1) }, 0);
			CreateRadialBurst(parent->m_childType, 0, parent, 2.5f, 0, vert, radius + 10, rotate, { RandFloat(0, 1),RandFloat(0, 1),RandFloat(0, 1) }, 0);
		}
		else if (parent->m_type == DOUBLE_CIRCLE_TRACE)
		{
			auto rotate = cyclone::Vector3(RandFloat(-1.0f, 1.0f), RandFloat(-1.0f, 1.0f), RandFloat(-1.0f, 1.0f));
			rotate.normalise();
			CreateRadialBurst(parent->m_childType, 0, parent, 2.5f, 0, RandInt(3, 10), RandDouble(5, 30), rotate, { RandFloat(0, 1),RandFloat(0, 1),RandFloat(0, 1) });
			CreateRadialBurst(parent->m_childType, 0, parent, 2.5f, 0, RandInt(3, 10), RandDouble(5, 30), rotate, { RandFloat(0, 1),RandFloat(0, 1),RandFloat(0, 1) });
		}
		else if (parent->m_type == FINALE)
		{
			auto rotate = cyclone::Vector3(1,1,0);
			rotate.normalise();
			std::vector<cyclone::Vector3> temp = GeneratePolygonVertics(20, rotate);
			std::vector<cyclone::Vector3> vertices;
			for (size_t i = 0; i < temp.size() / 2; i++)
			{
				CreateRadialBurst(parent->m_childType, 0, parent->m_position, 3.0f, 1.5f, 20, 20, temp[i], { RandFloat(0.5f, 1),RandFloat(0.5f, 1),RandFloat(0.5f, 1) }, 10);
				CreateRadialBurst(parent->m_childType, 0, parent->m_position, 3.0f, 1.5f, 20, 20, temp[i + (temp.size() / 2)], {RandFloat(0.5f, 1),RandFloat(0.5f, 1),RandFloat(0.5f, 1)}, 10);
			}

			rotate = cyclone::Vector3(RandFloat(-1.0f, 1.0f), RandFloat(-1.0f, 1.0f), RandFloat(-1.0f, 1.0f));
			int vert = 30;
			double radius = 30;
			CreateRadialBurst(parent->m_childType, 0, parent->m_position,4.5f, 1, vert, radius, rotate, { RandFloat(0.7, 1),RandFloat(0.8, 1),RandFloat(0.7, 1) }, 0);
		}
	}
};
#endif