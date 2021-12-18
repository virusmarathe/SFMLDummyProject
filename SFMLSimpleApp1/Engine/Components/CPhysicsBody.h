#pragma once

#include "Math/Vector2.h"

class CPhysicsBody
{
public:
	Vector2 velocity;
	bool elastic;

	CPhysicsBody() : velocity(Vector2(0,0)), elastic(false) { }
	CPhysicsBody(Vector2 vel) : velocity(vel), elastic(false) { }
	CPhysicsBody(Vector2 vel, bool elastic) : velocity(vel), elastic(elastic) { }
};