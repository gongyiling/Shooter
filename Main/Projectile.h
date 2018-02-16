#pragma once
#include "PxPhysicsAPI.h"
#include "Collider.h"
using namespace physx;

class Projectile : public Collider
{
public:
	void Init(PxVec3 spawn_position, int layer, PxVec3 velocity);
	void Step(float dt);
	void Destroy();
private:
	virtual void OnCollisionEnter(Collider* other) override;
private:
	PxRigidDynamic* m_rigid_dynamic = NULL;
};
