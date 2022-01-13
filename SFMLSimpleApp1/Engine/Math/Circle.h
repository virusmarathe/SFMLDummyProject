#pragma once

#include "Vector2.h"

struct Circle
{
	Vector2 pos;
	float radius;

	Circle(Vector2 pos, float radius) : pos(pos), radius(radius) {}
};