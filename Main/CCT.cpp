#include "CCT.h"
#include "Physics.h"
void CCT::Init(PxVec3 spawn_position, int layer)
{
	m_cct = createController();
	PxExtendedVec3 pos(spawn_position.x, spawn_position.y, spawn_position.z);
	m_cct->setFootPosition(pos);

	PxShape* shape = NULL;
	m_cct->getActor()->getShapes(&shape, 1);
	
	PxFilterData filter_data;
	filter_data.word0 = 1 << layer;
	shape->setQueryFilterData(filter_data);

	m_direction = FindRandomDirection();

	m_cd = 0;
}

void CCT::Step(float dt)
{
	float speed = 2;
	PxVec3 disp = m_direction * dt * speed;
	disp.y = -0.1f;
	PxControllerFilters filters;
	PxControllerCollisionFlags flags = m_cct->move(disp, 0.001, dt, filters);
	if (flags.isSet(PxControllerCollisionFlag::eCOLLISION_SIDES))
	{
		m_direction = FindRandomDirection();
	}
}

float Random11()
{
	return ((float)rand() / RAND_MAX * 2) - 1;
}

PxVec3 CCT::FindRandomDirection()
{
	PxVec3 d(PxIdentity);
	d.x = Random11();
	d.z = Random11();
	return d.getNormalized();
}

void CCT::Shoot(float dt)
{

}

