#pragma once

#include "PxPhysicsAPI.h"
#include <vector>

using namespace physx;

extern PxPhysics*				gPhysics;
extern PxScene*				gScene;
extern PxControllerManager*	gControllerManager;
extern PxMaterial*				gMaterial;
extern std::vector<class CCT*> gCCTs;

enum LayerType
{
	LT_OBSTACLE = 0,
	LT_LAYER0 = 1,
	LT_LAYER1 = 2,
};

extern int gCollisionMatrix[];

void initPhysics(bool interactive);
void stepPhysics(bool interactive);
void cleanupPhysics(bool interactive);
void setLayer(PxShape* shape, int layer);

PxVec3 getGravity();