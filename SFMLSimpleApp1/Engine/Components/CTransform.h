#pragma once
#include "Math/Vector2.h"

class CTransform
{
public:
	Vector2 position;
	Vector2 scale;
	Vector2 velocity;

	CTransform(Vector2 pos) : position(pos), scale(Vector2(1,1)) { }
	CTransform(Vector2 pos, Vector2 vel) : position(pos), velocity(vel), scale(Vector2(1, 1)) { }
	CTransform(Vector2 pos, Vector2 scale, Vector2 vel) : position(pos), velocity(vel), scale(scale) { }
};