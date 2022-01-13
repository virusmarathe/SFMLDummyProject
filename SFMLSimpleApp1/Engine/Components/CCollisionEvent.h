#pragma once
#include "Math/Vector2.h"
#include "Component.h"

class Entity;

class CCollisionEvent : public Component
{
public:

	std::shared_ptr<Entity> ent1;
	std::shared_ptr<Entity> ent2;
	Vector2 contactPoint, normal;
	float hitTime;

	CCollisionEvent(std::shared_ptr<Entity> ent1, std::shared_ptr<Entity> ent2, Vector2 contactPoint, Vector2 normal, float hitTime) :
		ent1(ent1), ent2(ent2), contactPoint(contactPoint), normal(normal), hitTime(hitTime) { }
};