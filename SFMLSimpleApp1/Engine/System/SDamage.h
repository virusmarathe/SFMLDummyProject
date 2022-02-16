#pragma once
#include "System.h"
#include "Physics/Physics.h"

class SDamage : public System
{
public:
	SDamage(EntityManager* entityManagerRef, int priority) : System(entityManagerRef, priority) { }

	virtual void update(float dt) override
	{
		for (auto ent : _entities->getEntities("CollisionEvent"))
		{
			std::shared_ptr<CCollisionEvent> cEvent = ent->getComponent<CCollisionEvent>();
			if (cEvent->ent1->hasComponent<CDamage>() && cEvent->ent2->hasComponent<CHealth>())
			{
				std::shared_ptr<CDamage> damageComponent = cEvent->ent1->getComponent<CDamage>();
				std::shared_ptr<CHealth> healthComponent = cEvent->ent2->getComponent<CHealth>();

				if ((healthComponent->hitLayer & damageComponent->layer) > 0)
				{
					healthComponent->lastHitTimer = 0;
					healthComponent->health -= damageComponent->damage;
					if (healthComponent->health <= 0)
					{
						cEvent->ent2->destroy();
					}
				}
			}
		}
	}
};
