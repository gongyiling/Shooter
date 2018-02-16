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
#include <vector>

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

std::vector<CCT*>					gCCTs;

int gCollisionMatrix[] = { -1, 
							-1 & ~(1 << LT_LAYER0),
							-1 & ~(1 << LT_LAYER1), };

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

void setArenaLayer()
{
	std::vector<PxActor*> actors(gScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC));
	gScene->getActors(PxActorTypeFlag::eRIGID_STATIC, &actors[0], actors.size());

	for (size_t i = 0; i < actors.size(); i++)
	{
		PxRigidActor* actor = actors[i]->isRigidActor();
		std::vector<PxShape*> shapes(actor->getNbShapes());
		actor->getShapes(&shapes[0], shapes.size());
		for (size_t j = 0; j < shapes.size(); j++)
		{
			setLayer(shapes[j], LT_OBSTACLE);
		}
	}
}

void createArena()
{
	// create playground
	gScene->addActor(*PxCreateStatic(*gPhysics, PxTransform(PxIdentity), PxBoxGeometry(10, 0.1, 10), *gMaterial));

	PxTransform offset(PxVec3(0, 1, 0), PxQuat(PxIdentity));
	PxBoxGeometry bound(1, 1, 10);
	for (int i = 0; i < 4; i++)
	{
		PxTransform transform;
		transform.q = PxQuat(PxHalfPi * i, PxVec3(0, 1, 0));
		transform.p = transform.q.rotate(PxVec3(10, 0, 0));
		gScene->addActor(*PxCreateStatic(*gPhysics, transform, bound, *gMaterial, offset));
	}

	PxBoxGeometry obstacle(1, 1, 6);
	gScene->addActor(*PxCreateStatic(*gPhysics, PxTransform(PxIdentity), obstacle, *gMaterial, offset));
	
	setArenaLayer();
}

void createCharacterController()
{
	CCT* cct = new CCT();

	cct->Init(PxVec3(5, 0, 0), LT_LAYER0);
	gCCTs.push_back(cct);

	cct = new CCT();
	cct->Init(PxVec3(-5, 0, 0), LT_LAYER1);
	gCCTs.push_back(cct);
}

class SimulationEventCallback : public PxSimulationEventCallback
{
public:

	virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
	{
	}


	virtual void onWake(PxActor** actors, PxU32 count)
	{

	}

	virtual void onSleep(PxActor** actors, PxU32 count)
	{

	}

	virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
	{
		for (PxU32 i = 0; i < nbPairs; i++)
		{
			const PxContactPair& pair = pairs[i];
			if (pair.events.isSet(PxPairFlag::eNOTIFY_TOUCH_FOUND))
			{
				Collider* first = (Collider*)pair.shapes[0]->userData;
				Collider* second = (Collider*)pair.shapes[1]->userData;
				if (first != NULL || second != NULL)
				{
					colliders_enter.push_back(std::make_pair(first, second));
				}
			}
		}
	}


	virtual void onTrigger(PxTriggerPair* pairs, PxU32 count)
	{

	}

	void Clear()
	{
		colliders_enter.clear();
	}

	std::vector<std::pair<Collider*, Collider*>> colliders_enter;
};

SimulationEventCallback gCallback;
PxFilterFlags MySimulateFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	if (!(filterData0.word0 & filterData1.word1))
	{
		return PxFilterFlag::eKILL;
	}

	if (!(filterData0.word1 & filterData1.word0))
	{
		return PxFilterFlag::eKILL;
	}

	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}

	pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
	return PxFilterFlag::eDEFAULT;
}

void createScene()
{
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = getGravity();
	gDispatcher = PxDefaultCpuDispatcherCreate(0);
	sceneDesc.cpuDispatcher = gDispatcher;
	//sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	sceneDesc.filterShader = MySimulateFilterShader;
	sceneDesc.simulationEventCallback = &gCallback;
	gScene = gPhysics->createScene(sceneDesc);

	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	gControllerManager = PxCreateControllerManager(*gScene);
	createArena();
	createCharacterController();
}

void processSimulateEvent()
{
	for (size_t i = 0; i < gCallback.colliders_enter.size(); i++)
	{
		std::pair<Collider*, Collider*> p = gCallback.colliders_enter[i];
		if (p.first != NULL)
		{
			p.first->OnCollisionEnter(p.second);
		}
		if (p.second != NULL)
		{
			p.second->OnCollisionEnter(p.first);
		}
	}
}

void stepPhysics(bool interactive)
{
	PX_UNUSED(interactive);
	float dt = 1 / 30.0f;
	for (size_t i = 0; i < gCCTs.size(); i++)
	{
		gCCTs[i]->Step(dt);
	}
	gCallback.Clear();

	gScene->simulate(dt);
	gScene->fetchResults(true);

	processSimulateEvent();
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

void setLayer(PxShape* shape, int layer)
{
	PxFilterData filter_data;
	filter_data.word0 = 1 << layer;
	shape->setQueryFilterData(filter_data);
	filter_data.word1 = gCollisionMatrix[layer];
	shape->setSimulationFilterData(filter_data);
}

PxVec3 getGravity()
{
	return PxVec3(0, -9.81f, 0);
}


