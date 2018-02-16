#include "CCT.h"
#include "Physics.h"
#include "Projectile.h"

PxController* CCT::CreateController()
{
	PxCapsuleControllerDesc desc;
	desc.radius = 0.5f;
	desc.height = 2 - desc.radius * 2;
	desc.stepOffset = 0.2f;
	desc.slopeLimit = PxCos(PxHalfPi * 0.5f);
	desc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
	desc.material = gMaterial;
	desc.userData = this;
	return gControllerManager->createController(desc);
}

void CCT::Init(PxVec3 spawn_position, int layer)
{
	m_cct = CreateController();
	PxExtendedVec3 pos(spawn_position.x, spawn_position.y, spawn_position.z);
	m_cct->setFootPosition(pos);

	PxShape* shape = NULL;
	m_cct->getActor()->getShapes(&shape, 1);
	
	setLayer(shape, m_layer);
	shape->userData = this;
	m_direction = FindRandomDirection();
	m_layer = layer;

	m_cd = 0;
}

void CCT::Step(float dt)
{
	float speed = 5;
	PxVec3 disp = m_direction * dt * speed;
	disp.y = -0.1f;
	PxControllerFilters filters;
	PxControllerCollisionFlags flags = m_cct->move(disp, 0.001, dt, filters);
	if (flags.isSet(PxControllerCollisionFlag::eCOLLISION_SIDES))
	{
		m_direction = FindRandomDirection();
	}
	Shoot(dt);
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
	if (m_cd <= 0)
	{
		m_cd = 5;
	}
	m_cd -= dt;
	if (m_cd > 0)
	{ 
		return;
	}
	else
	{
		DoShoot();
		DoProjectile();
	}
}

void CCT::DoShoot()
{
	PxVec3 origin = toVec3(m_cct->getPosition());
	PxRaycastBuffer hit;
	PxQueryFilterData query_filter_data;
	query_filter_data.data.word0 = ~(1 << m_layer);
	for (size_t i = 0; i < gCCTs.size(); i++)
	{
		if (gCCTs[i] == this)
		{
			continue;
		}

		PxVec3 other_origin = toVec3(gCCTs[i]->m_cct->getPosition());
		PxVec3 direction = (other_origin - origin).getNormalized();
		gScene->raycast(origin, direction, 100, hit, PxHitFlag::eDEFAULT, query_filter_data);
		if (!hit.hasBlock)
		{
			continue;
		}
		PxFilterData filter_data = hit.block.shape->getQueryFilterData();
		int layer_mask = (1 << LT_LAYER0) | (1 << LT_LAYER1);
		if (filter_data.word0 & layer_mask)
		{
			CCT* other = (CCT*)hit.block.shape->userData;
			other->Destroy();
			return;
		}
	}
}

void CCT::DoProjectile()
{
	Projectile* projectile = new Projectile();
	PxVec3 direction = m_direction;

	// x1 * x1 + z1 * z1 = 1
	// x2 * x2 + z2 * z2 = 1 - y2 * y2

	direction.y = PxSin(90.0f / 180 * PxPi);
	float n = PxSqrt(1 - direction.y * direction.y);
	direction.x *= n;
	direction.z *= n;
	PxVec3 velocity = direction * 5;
	projectile->Init(toVec3(m_cct->getPosition()) + PxVec3(0, 2, 0), m_layer, velocity);
}

void CCT::Destroy()
{
	auto it = std::find(gCCTs.begin(), gCCTs.end(), this);
	if (it != gCCTs.end())
	{
		m_cct->release();
		gCCTs.erase(it);
		delete this;
	}
}
