#pragma once
#include "System.h"
#include "Physics/Physics.h"

class SHealthBar : public System
{
public:
	SHealthBar(EntityManager* entityManagerRef, int priority) : System(entityManagerRef, priority) { }

	virtual void update(float dt) override
	{
		for (auto ent : _entities->getEntities())
		{
			if (ent.hasComponent<CHealth>())
			{
				ent.getComponent<CHealth>().rendered = ent.getComponent<CHealth>().lastHitTimer < 3.0f;
				float ratio = ent.getComponent<CHealth>().health / ent.getComponent<CHealth>().maxHealth;
				ent.getComponent<CHealth>().foreground.setScale(sf::Vector2f(ratio, 1));
				ent.getComponent<CHealth>().lastHitTimer += dt;
			}
		}
	}
};
