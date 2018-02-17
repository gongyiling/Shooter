#pragma once
#include "PxPhysicsAPI.h"
#include "Collider.h"

using namespace physx;

class Door : public Collider
{
public:
	void Init(PxVec3 spawn_position, int layer);
	void OpenDoor();
private:
	PxRigidDynamic* m_door = NULL;
	PxRevoluteJoint* m_joint = NULL;
};
