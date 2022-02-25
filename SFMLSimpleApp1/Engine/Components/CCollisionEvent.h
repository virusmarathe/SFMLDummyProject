#pragma once
#include "Math/Vector2.h"
#include "Component.h"

class CCollisionEvent : public Component
{
public:

	size_t ent1 = 0;
	size_t ent2 = 0;
	Vector2 contactPoint, normal;
	float hitTime = 0;

	CCollisionEvent() {}
	CCollisionEvent(size_t ent1, size_t ent2, Vector2 contactPoint, Vector2 normal, float hitTime) :
		ent1(ent1), ent2(ent2), contactPoint(contactPoint), normal(normal), hitTime(hitTime) { }
};