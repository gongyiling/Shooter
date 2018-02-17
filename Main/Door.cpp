#include "Door.h"
#include "Physics.h"

void Door::Init(PxVec3 spawn_position, int layer)
{
	PxTransform transform(spawn_position, PxQuat(PxIdentity));
	m_door = gPhysics->createRigidDynamic(transform);
	PxBoxGeometry box(1, 1, 0.1f);
	PxShape* shape = m_door->createShape(box, *gMaterial, PxTransform(PxVec3(0, 1.1f, 0), PxQuat(PxIdentity)));
	setLayer(shape, layer);
	shape->userData = this;
	m_door->setSolverIterationCounts(10, 1);

	PxTransform joint_anchor0(spawn_position, PxQuat(PxHalfPi, PxVec3(0, 0, 1)));

	PxTransform joint_anchor1(PxVec3(0, 0, 0), joint_anchor0.q);
	m_joint = PxRevoluteJointCreate(*gPhysics, nullptr, joint_anchor0, m_door, joint_anchor1);

	gScene->addActor(*m_door);
	m_type = "Door";
}

void Door::OpenDoor()
{
	m_door->setAngularVelocity(PxVec3(0, 1, 0) * 5);
}
