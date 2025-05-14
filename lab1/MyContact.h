#pragma once

#include <vector>
#include "particle.h"
#include "plinks.h"

namespace cyclone
{
	class MyGroundContact : public cyclone::ParticleContactGenerator
	{
		std::vector<cyclone::Particle*> m_particles; //해당되는 파티클이 들어 있는 컨테이너
		double m_size; //파티클 크기 (일단 모든 파티클의 크기가 동일하다고 가정)
	public:
		MyGroundContact();
		~MyGroundContact();
		void init(cyclone::Particle* p, double size); //파티클 p를 컨테이너에 추가 함 (push_back())
		//충돌을 감지해서, 만일 충돌이 일어났다면, contact에 필요한 정보를 저장(limit = 가장 최대로 일어날 충돌 수)
		//충돌이 발생했다면, 전체 발생한 충돌의 횟수를 반환 함
		virtual unsigned addContact(cyclone::ParticleContact* contact, unsigned limit) const;
	};
}