#include "CCT.h"
#include "Physics.h"
void CCT::Init()
{
	m_cct = createController();
	m_direction = FindRandomDirection();
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

