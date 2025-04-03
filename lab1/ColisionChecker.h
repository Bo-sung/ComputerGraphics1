#ifndef __COLLISIONCHECKER_H
#define __COLLISIONCHECKER_H

#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "core.h"

class ColisionChecker
{
private:
	/// <summary>
	/// 밑면 벡터
	/// </summary>
	static void CalBaseVector(cyclone::Vector3 _p1, cyclone::Vector3 _p2, cyclone::Vector3 _p3, cyclone::Vector3* _v1, cyclone::Vector3* _v2)
	{
		_v1->x = _p2.x - _p1.x;
		_v1->y = _p2.y - _p1.y;
		_v1->z = _p2.z - _p1.z;

		_v2->x = _p3.x - _p1.x;
		_v2->y = _p3.y - _p1.y;
		_v2->z = _p3.z - _p1.z;
	}

	/// <summary>
	/// 외적 계산
	/// </summary>
	static cyclone::Vector3 CalCrossProduct(cyclone::Vector3 _v1, cyclone::Vector3 _v2)
	{
		cyclone::Vector3 crossProduct = cyclone::Vector3();
		crossProduct.x = _v1.y * _v2.z - _v1.z * _v2.y;
		crossProduct.y = _v1.z * _v2.x - _v1.x * _v2.z;
		crossProduct.z = _v1.x * _v2.y - _v1.y * _v2.x;
		return crossProduct;
	}

	/// <summary>
	/// 평면 방정식 계산
	/// </summary>
	static double CalPlaneEquation(cyclone::Vector3 _p, cyclone::Vector3 _normal, float* D)
	{
		*D = -(_normal.x * _p.x + _normal.y * _p.y + _normal.z * _p.z);
		return sqrt(_normal.x * _normal.x + _normal.y * _normal.y + _normal.z * _normal.z);
	}

public:
	static const int NOCOLLISION = 0;
	static const int COLLISION = 1;
	static const int OVERLAP = 2;

	/// <summary>
	/// 한 점에서 면까지의 거리. 
	/// </summary>
	/// <param name="_s_origin"> 점</param>
	/// <param name="_p_vertexs"> 면, 면을 구성하는 정점의 갯수가 3 미만인 경우 크래시. (면 미성립)</param>
	/// <returns> 점에서 면까지 최소 거리</returns>
	static float CheckDistance_Point_Plane(cyclone::Vector3 _s_origin, cyclone::Vector3* _p_vertexs)
	{
		// 삼각뿔의 높이 구하는 공식 적용
		cyclone::Vector3 v1 = cyclone::Vector3();
		cyclone::Vector3 v2 = cyclone::Vector3();
		// 밑면 벡터
		CalBaseVector(_p_vertexs[0], _p_vertexs[1], _p_vertexs[2], &v1, &v2);

		// 외적 계산
		cyclone::Vector3 normal = CalCrossProduct(v1, v2);

		float d;
		// 평면 방정식 계산
		double plainMagnitude = CalPlaneEquation(_p_vertexs[0], normal, &d);

		float numerator = fabs(normal.x * _s_origin.x + normal.y * _s_origin.y + normal.z * _s_origin.z + d);
		return numerator / plainMagnitude;
	}

	/// <summary>
	/// 구 면 충돌 체크.
	/// </summary>
	/// <param name="_s_origin">구 중점</param>
	/// <param name="_s_size">구 반지름</param>
	/// <param name="_p_vertexs">면. 면을 구성하는 정점의 갯수가 3 미만인 경우 크래시. (면 미성립)</param>
	/// <returns>
	///		OVERLAP : 겹침.
	///		NOCOLLISION : 미 충돌
	///		COLLISION : 한점에서 접함.
	/// </returns>
	static int CheckCollision_Sphere_Plane(cyclone::Vector3 _s_origin, float _s_size, cyclone::Vector3* _p_vertexs)
	{
		float distance = CheckDistance_Point_Plane(_s_origin, _p_vertexs);

		if (distance < _s_size)
			return OVERLAP;
		else if (distance > _s_size)
			return NOCOLLISION;
		else
			return COLLISION;
	}

	/// <summary>
	/// 구 면 충돌 여부 리턴.
	/// </summary>
	/// <param name="_s_origin">구 중점</param>
	/// <param name="_s_size">구 반지름</param>
	/// <param name="_p_vertexs">면, 면을 구성하는 정점의 갯수가 3 미만인 경우 크래시. (면 미성립)</param>
	/// <returns>
	///		중점과 면의 거리보다 반지름이 크거나 같으면 True
	/// </returns>
	static bool IsCollision_Sphere_Plane(cyclone::Vector3 _s_origin, float _s_size, cyclone::Vector3* _p_vertexs)
	{
		return _s_size >= CheckDistance_Point_Plane(_s_origin, _p_vertexs);
	}
};

#endif