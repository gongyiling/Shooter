#pragma once
#include "PxPhysicsAPI.h"
#include "characterkinematic/PxCapsuleController.h"
#include "Collider.h"

using namespace physx;

class CCT : public Collider, public PxUserControllerHitReport
{
public:
	void Init(PxVec3 spawn_position, int layer);
	void Step(float dt);
	void Destroy();
private:
	PxController* CreateController();
	PxVec3 FindRandomDirection();
	void Shoot(float dt);
	void DoShoot();
	void DoProjectile();
private:
	virtual void onShapeHit(const PxControllerShapeHit& hit);

	virtual void onControllerHit(const PxControllersHit& hit);

	virtual void onObstacleHit(const PxControllerObstacleHit& hit);
private:
	PxController* m_cct = NULL;
	PxVec3 m_direction;
	float m_cd;
	int m_layer;
};
