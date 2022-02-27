#pragma once

#include "Math/Rect.h"
#include "Component.h"

struct CRectCollider : public Component
{
	Rect rect;
	Vector2 offset;
	bool isTrigger = false;
	int collisionLayer = 1;
};