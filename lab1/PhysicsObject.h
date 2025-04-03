#ifndef __PHYSICSOBJECT_H
#define __PHYSICSOBJECT_H

#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "core.h"
#include "ColisionChecker.h"

class PhysicsObject
{
private:
	bool m_isGravity;
	cyclone::Vector3 m_position;
	float m_mass;
	cyclone::Vector3 m_velocity;
	cyclone::Vector3 m_acceleration;
	float m_damping;

	cyclone::Vector3 m_forceAccum;

	void Init()
	{
		m_position = cyclone::Vector3(0, 0, 0);
		m_mass = 0.0f;
		m_velocity = cyclone::Vector3(0, 0, 0);
		m_acceleration = cyclone::Vector3(0, 0, 0);
		m_damping = 0.0f;
	}
public:
	PhysicsObject(bool _isGravity = false)
	{
		Init();
		m_isGravity = _isGravity;
	}
	//PhysicsObject()
	//{
	//	Init();
	//};
	~PhysicsObject() {};

	void integrate(float _delta_t)
	{
		cyclone::Vector3 resultAcc = m_acceleration;
		
		resultAcc.addScaledVector(m_forceAccum, m_mass);
		m_velocity.addScaledVector(resultAcc, _delta_t);

		// Impose drag Àû¿ë
		m_velocity *= real_pow(m_damping, _delta_t);

		m_position.addScaledVector(m_velocity, _delta_t);

		m_forceAccum.clear();
	}

	void SetPosition(cyclone::Vector3 _position)
	{
		m_position = _position;
	}
	cyclone::Vector3 GetPosition()
	{
		return m_position;
	}

	void SetMass(float _mass)
	{
		m_mass = _mass;
	}
	float GetMass()
	{
		return m_mass;
	}

	void SetVelocity(cyclone::Vector3 _velocity)
	{
		m_velocity = _velocity;
	}
	cyclone::Vector3 GetVelocity()
	{
		return m_velocity;
	}

	void SetAcceleration(cyclone::Vector3 _acceleration)
	{
		m_acceleration = _acceleration;
	}
	void ApplyAcceleration(cyclone::Vector3 _acceleration)
	{
		m_acceleration += _acceleration;
	}
	void AddForce(cyclone::Vector3 _force)
	{
		m_forceAccum += _force;
	}

	cyclone::Vector3 GetAcceleration()
	{
		return m_acceleration;
	}

	void SetDamping(float _damping)
	{
		m_damping = _damping;
	}
	float GetDamping()
	{
		return m_damping;
	}
};
#endif