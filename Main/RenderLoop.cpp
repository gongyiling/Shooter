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


#include <vector>

#include "PxPhysicsAPI.h"

#include "../Render/Render.h"
#include "../Render/Camera.h"

using namespace physx;

extern void initPhysics(bool interactive);
extern void stepPhysics(bool interactive);	
extern void cleanupPhysics(bool interactive);
extern void keyPress(const char key, const PxTransform& camera);


namespace
{
Renderer::Camera*	sCamera;

void motionCallback(int x, int y)
{
	sCamera->handleMotion(x, y);
}

void keyboardCallback(unsigned char key, int x, int y)
{
	if(key==27)
		exit(0);

	if(!sCamera->handleKey(key, x, y))
		keyPress(key, sCamera->getTransform());
}

void mouseCallback(int button, int state, int x, int y)
{
	sCamera->handleMouse(button, state, x, y);
}

void idleCallback()
{
	glutPostRedisplay();
}

void renderCallback()
{
	stepPhysics(true);

	Renderer::startRender(sCamera->getEye(), sCamera->getDir());

	PxScene* scene;
	PxGetPhysics().getScenes(&scene,1);
	PxU32 nbActors = scene->getNbActors(PxActorTypeSelectionFlag::eRIGID_DYNAMIC | PxActorTypeSelectionFlag::eRIGID_STATIC);
	if(nbActors)
	{
		std::vector<PxRigidActor*> actors(nbActors);
		scene->getActors(PxActorTypeSelectionFlag::eRIGID_DYNAMIC | PxActorTypeSelectionFlag::eRIGID_STATIC, (PxActor**)&actors[0], nbActors);
		Renderer::renderActors(&actors[0], (PxU32)actors.size(), true);
	}

	Renderer::finishRender();
}

void exitCallback(void)
{
	delete sCamera;
	cleanupPhysics(true);
}
}

void renderLoop()
{
	sCamera = new Renderer::Camera(PxVec3(50.0f, 50.0f, 50.0f), PxVec3(-0.6f,-0.2f,-0.7f));

	Renderer::setupDefaultWindow("PhysX Snippet HelloWorld");
	Renderer::setupDefaultRenderState();

	glutIdleFunc(idleCallback);
	glutDisplayFunc(renderCallback);
	glutKeyboardFunc(keyboardCallback);
	glutMouseFunc(mouseCallback);
	glutMotionFunc(motionCallback);
	motionCallback(0,0);

	atexit(exitCallback);

	initPhysics(true);
	glutMainLoop();
}

