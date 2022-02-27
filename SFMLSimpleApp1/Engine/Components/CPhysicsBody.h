#pragma once

#include "Math/Vector2.h"
#include "Component.h"

struct CPhysicsBody : public Component
{
	Vector2 velocity;
	bool elastic = false;
	int collidesWith = 0;
};