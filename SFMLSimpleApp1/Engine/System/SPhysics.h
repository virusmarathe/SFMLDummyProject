#pragma once

#include "System.h"
#include "Physics/Physics.h"

class SPhysics : public System
{
public:
	SPhysics(EntityManager* entityManagerRef, int priority) : System(entityManagerRef, priority) { }

	virtual void update(float dt) override
	{
        Vector2 contactPoint;
        Vector2 normal;
        float hitTime;

        EntityList entities = _entities->getEntitiesByType<CPhysicsBody>();
        EntityList colliders = _entities->getEntitiesByType<CRectCollider>();

        for (auto ent : entities)
        {
            if (ent.hasComponent<CRectCollider>())
            {
                auto& ent1Collider = ent.getComponent<CRectCollider>();
                auto& physics = ent.getComponent<CPhysicsBody>();
                if (!ent1Collider.enabled || physics.velocity == Vector2(0,0) || physics.collidesWith == 0) continue;

                for (auto ent2 : colliders)
                {
                    if (ent == ent2) continue;
                    auto& ent2Collider = ent2.getComponent<CRectCollider>();
                    if (!ent2Collider.enabled) continue;
                    if ((ent2Collider.collisionLayer & physics.collidesWith) == 0) continue;
                    if (ent1Collider.isTrigger)
                    {
                        if (Physics::checkCollision(ent1Collider.rect, ent2Collider.rect))
                        {
                            Entity collisionEntity = _entities->addEntity("CollisionEvent");
                            auto& collisionEvent = collisionEntity.addComponent<CCollisionEvent>();
                            collisionEvent.ent1 = ent.id;
                            collisionEvent.ent2 = ent2.id;
                        }
                    }
                    else
                    {
                        if (Physics::checkCollision(ent1Collider.rect, physics.velocity * dt, ent2Collider.rect, contactPoint, normal, hitTime))
                        {
                            Entity collisionEntity = _entities->addEntity("CollisionEvent");
                            auto& collisionEvent = collisionEntity.addComponent<CCollisionEvent>();
                            collisionEvent.ent1 = ent.id;
                            collisionEvent.ent2 = ent2.id;
                            collisionEvent.contactPoint = contactPoint;
                            collisionEvent.normal = normal;
                            collisionEvent.hitTime = hitTime;

                            if (!ent2Collider.isTrigger)
                            {
                                if (physics.elastic)
                                {
                                    // calculate reflection vector
                                    physics.velocity = (normal * (-1 * physics.velocity.dot(normal)) * 2) + physics.velocity;
                                }
                                else
                                {
                                    physics.velocity -= (normal * (physics.velocity * dt * (1 - hitTime)).dot(normal)) / dt;
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }
	}
};