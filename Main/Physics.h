#pragma once

#include "PxPhysicsAPI.h"

using namespace physx;

void initPhysics(bool interactive);
void stepPhysics(bool interactive);
void cleanupPhysics(bool interactive);

PxController* createController();

PxVec3 getGravity();