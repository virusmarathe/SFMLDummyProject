#pragma once

#include "System.h"
#include "Physics/Physics.h"

class SPhysics : public System
{
public:
	SPhysics(EntityManager* entityManagerRef, int priority) : System(entityManagerRef, priority) { }

	virtual void update(float dt) override
	{
        for (auto ent : _entities->getEntities())
        {
            if (ent->hasComponent<CRectCollider>() && ent->hasComponent<CPhysicsBody>())
            {
                std::shared_ptr<CRectCollider> ent1Collider = ent->getComponent<CRectCollider>();
                if (!ent1Collider->enabled) continue;

                for (auto ent2 : _entities->getEntities())
                {
                    if (ent == ent2) continue;
                    if (ent2->hasComponent<CRectCollider>())
                    {
                        std::shared_ptr<CRectCollider> ent2Collider = ent2->getComponent<CRectCollider>();
                        if (!ent2Collider->enabled) continue;

                        std::shared_ptr<CPhysicsBody> physics = ent->getComponent<CPhysicsBody>();

                        Vector2 contactPoint;
                        Vector2 normal;
                        float hitTime;

                        if (Physics::checkCollision(ent1Collider->rect, physics->velocity * dt, ent2Collider->rect, contactPoint, normal, hitTime))
                        {
                            std::shared_ptr<Entity> collisionEntity = _entities->addEntity("CollisionEvent");
                            collisionEntity->addComponent<CCollisionEvent>(ent, ent2, contactPoint, normal, hitTime);
                            collisionEntity->destroy(); // auto destroy events at end of frame                        

                            if (!ent2Collider->isTrigger && !ent1Collider->isTrigger)
                            {
                                if (physics->elastic)
                                {
                                    // calculate reflection vector
                                    physics->velocity = (normal * (-1 * physics->velocity.dot(normal)) * 2) + physics->velocity;
                                }
                                else
                                {
                                    physics->velocity -= (normal * (physics->velocity * dt * (1 - hitTime)).dot(normal)) / dt;
                                }
                            }
                        }
                    }
                }
            }
        }
	}
};