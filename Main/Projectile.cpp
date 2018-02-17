#include "Projectile.h"
#include "Physics.h"

void Projectile::Init(PxVec3 spawn_position, int layer, PxVec3 velocity)
{
	PxTransform transform(spawn_position, PxQuat(PxIdentity));
	PxSphereGeometry sphere(0.1f);
	m_rigid_dynamic = gPhysics->createRigidDynamic(transform);
	PxShape* shape = m_rigid_dynamic->createShape(sphere, *gMaterial, PxTransform(PxIdentity));
	setLayer(shape, layer);
	m_rigid_dynamic->setLinearVelocity(velocity);
	m_rigid_dynamic->userData = this;
	//m_rigid_dynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	shape->userData = this;
	gScene->addActor(*m_rigid_dynamic);
}

void Projectile::Step(float dt)
{

}

void Projectile::OnCollisionEnter(Collider* other)
{
	Destroy();
}

void Projectile::Destroy()
{
	gScene->removeActor(*m_rigid_dynamic);
}
