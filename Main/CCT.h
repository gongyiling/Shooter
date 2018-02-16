#pragma once
#include "PxPhysicsAPI.h"
#include "characterkinematic/PxCapsuleController.h"

using namespace physx;

class CCT
{
public:
	void Init();
	void Step(float dt);
private:
	PxVec3 FindRandomDirection();
private:
	PxController* m_cct = NULL;
	PxVec3 m_direction;
};
