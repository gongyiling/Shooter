#pragma once

#include "PxPhysicsAPI.h"

using namespace physx;

void initPhysics(bool interactive);
void stepPhysics(bool interactive);
void cleanupPhysics(bool interactive);
void keyPress(const char key, const PxTransform& camera);

void createScene();
