#define NOMINMAX
#include <iostream>
#include "core.h"

int main()
{
	//cyclone::Vector3 p = cyclone::Vector3(1, 4, 5);
	//std::cout << "p : " << p.toString() << std::endl;
	//std::cout << "p + (5,1,4) = " << (p + cyclone::Vector3(5, 1, 4)).toString() << std::endl;
	//
	//cyclone::Vector3 v1 = cyclone::Vector3(1, 0, 0);
	//cyclone::Vector3 v2 = cyclone::Vector3(0, 1, 0);
	//std::cout << "v1 : " << v1.toString() << " v2 : " << v2.toString() << std::endl;
	//std::cout << "v1 + v2 = " << (v1 + v2).toString() << std::endl;
	//
	//v1 = cyclone::Vector3(0, 1, 0);
	//std::cout << "v1 : " << v1.toString() << std::endl;
	//std::cout << "v1 * 5 = " << (v1 * 5).toString() << std::endl;
	//
	//cyclone::Vector3 start = cyclone::Vector3(8, 2, 5);
	//cyclone::Vector3 end = cyclone::Vector3(1, -1, 4);
	//std::cout << "start : " << start.toString() << std::endl;
	//std::cout << "end : " << end.toString() << std::endl;
	//std::cout << "result : " << (end - start).toString() << std::endl;
	//
	//cyclone::Vector3 v = cyclone::Vector3(1, 3, 4);
	//std::cout << "v : " << v.toString() << std::endl;
	//v.normalise();
	//std::cout << "v (Normalized) : " << v.toString() << std::endl;

	//const double M_PI = 3.141592;
	//const double DEGREES_TO_RADIAN = M_PI / 180.0f;
	//const double RADIANS_TO_DEGREES = 180.0f / M_PI;
	//
	//cyclone::Vector3 v(2, -1, 1);
	//cyclone::Vector3 u(1, 1, 2);
	//float d = v.dot(u);
	//double c = d / (u.magnitude() * v.magnitude());
	//double deg = acos(c) * RADIANS_TO_DEGREES;
	//
	//std::cout << deg << std::endl;

	//cyclone::Vector3 A(1, 3, 4);
	//cyclone::Vector3 B(-1,2,7);
	//
	//cyclone::Vector3 C = A.cross(B);
	//cyclone::Vector3 C1 = A.vectorProduct(B);
	//cyclone::Vector3 C2 = A % B;
	//
	//std::cout << C.toString() << std::endl;
	//std::cout << C1.toString() << std::endl;
	//std::cout << C2.toString() << std::endl;

	//cyclone::Vector3 U(1, -2, -1), V(-2, 4, 1);
	//
	//cyclone::Vector3 UV = U.cross(V);
	//
	//std::cout << UV.toString() << std::endl;

	//cyclone::Vector3 A(1, 2, -1), B(3, 0, 2);
	//
	//std::cout << "A : " << A.toString() << std::endl;
	//std::cout << "B : " << B.toString() << std::endl;
	//
	//cyclone::Vector3 C;
	//A.normalise();
	//C = A.cross(B);
	//// 평행 여부 체크
	//if (C.magnitude() == 0)
	//{
	//
	//}
	//else
	//{
	//	C.normalise();
	//	B = C.cross(A);
	//}
	//
	//std::cout << "A : " << A.toString() << std::endl;
	//std::cout << "B : " << B.toString() << std::endl;
	//std::cout << "C : " << C.toString() << std::endl;

	cyclone::Matrix3 M
	{
		1,2,3,
		1,0,2,
		3,1,2
	};

	std::cout << M.transform(cyclone::Vector3(1, 2, 0)).toString() << std::endl;

	return 0;
}
