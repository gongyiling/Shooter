/*
 * Copyright (c) 2008-2015, NVIDIA CORPORATION.  All rights reserved.
 *
 * NVIDIA CORPORATION and its licensors retain all intellectual property
 * and proprietary rights in and to this software, related documentation
 * and any modifications thereto.  Any use, reproduction, disclosure or
 * distribution of this software and related documentation without an express
 * license agreement from NVIDIA CORPORATION is strictly prohibited.
 */
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

// ****************************************************************************
// This snippet illustrates simple use of physx
//
// It creates a number of box stacks on a plane, and if rendering, allows the
// user to create new stacks and fire a ball from the camera position
// ****************************************************************************

#include <ctype.h>

#include "PxPhysicsAPI.h"
#include "Physics.h"
#include "CCT.h"

using namespace physx;

PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;

PxFoundation*			gFoundation = NULL;
PxPhysics*				gPhysics	= NULL;

PxDefaultCpuDispatcher*	gDispatcher = NULL;
PxScene*				gScene		= NULL;

PxControllerManager*	gControllerManager = NULL;

PxMaterial*				gMaterial	= NULL;

PxVisualDebuggerConnection*		
						gConnection	= NULL;

CCT*					gCCT = NULL;

void createScene();

void initPhysics(bool interactive)
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	PxProfileZoneManager* profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(gFoundation);
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(),true,profileZoneManager);
	
	if(gPhysics->getPvdConnectionManager())
	{
		gPhysics->getVisualDebugger()->setVisualizeConstraints(true);
		gPhysics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_CONTACTS, true);
		gPhysics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES, true);
		gConnection = PxVisualDebuggerExt::createConnection(gPhysics->getPvdConnectionManager(), "127.0.0.1", 5425, 10);
	}

	createScene();
}

void createArena()
{
	// create playground
	gScene->addActor(*PxCreateStatic(*gPhysics, PxTransform(PxIdentity), PxBoxGeometry(10, 0.1, 10), *gMaterial));

	PxBoxGeometry box(0.5f, 0.5f, 10);
	for (int i = 0; i < 4; i++)
	{
		PxTransform transform;
		transform.q = PxQuat(PxHalfPi * i, PxVec3(0, 1, 0));
		transform.p = transform.q.rotate(PxVec3(10, 0, 0));
		PxTransform offset(PxVec3(0, 0.5, 0), PxQuat(PxIdentity));
		gScene->addActor(*PxCreateStatic(*gPhysics, transform, box, *gMaterial, offset));
	}
}

void createCharacterController()
{
	gCCT = new CCT();
	gCCT->Init();
}

void createScene()
{
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = getGravity();
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	gScene = gPhysics->createScene(sceneDesc);

	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	gControllerManager = PxCreateControllerManager(*gScene);
	createArena();
	createCharacterController();
}


void stepPhysics(bool interactive)
{
	PX_UNUSED(interactive);
	float dt = 1 / 60.f;
	gCCT->Step(dt);
	gScene->simulate(dt);
	gScene->fetchResults(true);
	
}
	
void cleanupPhysics(bool interactive)
{
	PX_UNUSED(interactive);
	gScene->release();
	gDispatcher->release();
	PxProfileZoneManager* profileZoneManager = gPhysics->getProfileZoneManager();
	if(gConnection != NULL)
		gConnection->release();
	gPhysics->release();	
	profileZoneManager->release();
	gFoundation->release();
	
	printf("SnippetHelloWorld done.\n");
}

PxController* createController()
{
	PxCapsuleControllerDesc desc;
	desc.radius = 0.5f;
	desc.height = 2 - desc.radius * 2;
	desc.stepOffset = 0.2f;
	desc.slopeLimit = PxCos(PxHalfPi * 0.5f);
	desc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
	desc.material = gMaterial;
	return gControllerManager->createController(desc);
}

PxVec3 getGravity()
{
	return PxVec3(0, -9.81f, 0);
}


