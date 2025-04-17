#ifndef __MY_SPRING_H
#define __MY_SPRING_H

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <core.h>
#include "particle.h"

#include <pfgen.h>

namespace cyclone {
	class MySpring : public cyclone::ParticleForceGenerator
	{
		cyclone::Particle* other;
		double springConstant;
		double restLength;
	public:
		MySpring(cyclone::Particle* p, double springConstant, double restLength);
		virtual void updateForce(cyclone::Particle* p, double duration);
	};

	class MyAnchoredSpring : public cyclone::ParticleForceGenerator
	{
	protected:
		/// <summary>
		/// 스프링 고정 위치
		/// </summary>
		cyclone::Vector3* anchor;
		/// <summary>
		/// 스프링 상수
		/// </summary>
		double springConstant;
		/// <summary>
		///  rest 길이
		/// </summary>
		double restLength;
	public:
		/// <summary>
		/// 생성자
		/// </summary>
		MyAnchoredSpring();
		/// <summary>
		/// 생성자
		/// </summary>
		/// <param name="anchor">앵커 위치</param>
		/// <param name="springConstant"> 스프링 상수</param>
		/// <param name="restLength">rest 값</param>
		MyAnchoredSpring(Vector3* anchor, double springConstant, double restLength);
		/// <summary>
		/// 앵커 위치 반환
		/// </summary>
		/// <returns></returns>
		const Vector3* getAnchor() const { return anchor; }
		/// <summary>
		/// 값 설정 함수
		/// </summary>
		/// <param name="anchor"></param>
		/// <param name="springConstant"></param>
		/// <param name="restLength"></param>
		void init(Vector3* anchor, double springConstant, double restLength);
		/// <summary>
		/// 힘 업뎃
		/// </summary>
		/// <param name="particle"></param>
		/// <param name="duration"></param>
		virtual void updateForce(Particle* particle, real duration);
	};
}
#endif 