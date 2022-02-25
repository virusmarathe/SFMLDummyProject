#pragma once

#include "System.h"

class SMovement : public System
{
public:
	SMovement(EntityManager* entityManagerRef, int priority): System(entityManagerRef, priority) { }

	virtual void update(float dt) override
	{
        for (auto ent : _entities->getEntities())
        {
            if (ent.hasComponent<CTransform>() && ent.hasComponent<CPhysicsBody>())
            {
                ent.getComponent<CTransform>().position += ent.getComponent<CPhysicsBody>().velocity * dt;
                if (ent.hasComponent<CRectCollider>())
                {
                    ent.getComponent<CRectCollider>().rect.pos = ent.getComponent<CTransform>().position + ent.getComponent<CRectCollider>().offset;
                }
            }
        }
	}
};