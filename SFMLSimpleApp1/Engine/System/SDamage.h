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
			auto& cEvent = ent.getComponent<CCollisionEvent>();
			Entity ent1 = (*_entities)[cEvent.ent1];
			Entity ent2 = (*_entities)[cEvent.ent2];

			if (ent1.hasComponent<CDamage>() && ent2.hasComponent<CHealth>())
			{
				auto& damageComponent = ent1.getComponent<CDamage>();
				auto& healthComponent = ent2.getComponent<CHealth>();

				if ((healthComponent.hitLayer & damageComponent.layer) > 0)
				{
					healthComponent.lastHitTimer = 0;
					healthComponent.health -= damageComponent.damage;
					if (healthComponent.health <= 0)
					{
						ent2.destroy();
					}
				}
			}			
		}
	}
};
