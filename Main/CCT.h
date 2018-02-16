#pragma once
#include "PxPhysicsAPI.h"
#include "characterkinematic/PxCapsuleController.h"

using namespace physx;

class CCT
{
public:
	void Init(PxVec3 spawn_position, int layer);
	void Step(float dt);
private:
	PxVec3 FindRandomDirection();
	void Shoot(float dt);
private:
	PxController* m_cct = NULL;
	PxVec3 m_direction;
	float m_cd;
};
