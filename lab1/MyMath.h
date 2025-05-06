#ifndef __MYMATH_H
#define __MYMATH_H

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

const struct GLfloat3
{
	GLfloat r;
	GLfloat g;
	GLfloat b;
};

class MyMath
{
public:
	/// <summary>
	/// 랜덤 float 생성
	/// </summary>
	const float RandFloat(float min, float max)
	{
		return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
	}

	/// <summary>
	/// 랜덤 벡터 생성
	/// </summary>
	const cyclone::Vector3 MakeRandVec(float _min_x, float _min_y, float _min_z, float _max_x, float _max_y, float _max_z)
	{
		return MakeRandVec(cyclone::Vector3(_min_x, _min_y, _min_z), cyclone::Vector3(_max_x, _max_y, _max_z));
	}

	/// <summary>
	/// 랜덤 벡터 생성
	/// </summary>
	const cyclone::Vector3 MakeRandVec(cyclone::Vector3 _min, cyclone::Vector3 _max)
	{
		return cyclone::Vector3
		(
			RandFloat(_min.x, _max.x),
			RandFloat(_min.y, _max.y),
			RandFloat(_min.z, _max.z)
		);
	}
	const GLfloat3 FadeOutColor(GLfloat3 _color, float _delta_t, float _fadeSpeed = 1.0f)
	{
		_color.r = std::max(0.2f, _color.r - _delta_t * _fadeSpeed);
		_color.g = std::max(0.2f, _color.g - _delta_t * _fadeSpeed);
		_color.b = std::max(0.2f, _color.b - _delta_t * _fadeSpeed);

		return _color;
	}

	/// <summary>
	/// 랜덤 float 생성
	/// </summary>
	const float RandFloat(float min, float max)
	{
		return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
	}

	/// <summary>
	/// 랜덤 벡터 생성
	/// </summary>
	const cyclone::Vector3 MakeRandVec(float _min_x, float _min_y, float _min_z, float _max_x, float _max_y, float _max_z)
	{
		return MakeRandVec(cyclone::Vector3(_min_x, _min_y, _min_z), cyclone::Vector3(_max_x, _max_y, _max_z));
	}

	/// <summary>
	/// 랜덤 벡터 생성
	/// </summary>
	const cyclone::Vector3 MakeRandVec(cyclone::Vector3 _min, cyclone::Vector3 _max)
	{
		return cyclone::Vector3
		(
			RandFloat(_min.x, _max.x),
			RandFloat(_min.y, _max.y),
			RandFloat(_min.z, _max.z)
		);
	}


	// 로드리게스 회전 공식을 이용한 벡터 회전 함수
	const cyclone::Vector3 rotate_vector_rodrigues(cyclone::Vector3 axis, cyclone::Vector3 vector, double angle_radians)
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
	const std::vector<cyclone::Vector3> GeneratePolygonVertics(int sides, cyclone::Vector3 rotation)
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

	const std::vector<cyclone::Vector3> GenerateSphereVertics(int sides, cyclone::Vector3 rotation)
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
public:
};
#endif