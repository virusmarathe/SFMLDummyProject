#pragma once

#include "Math/Vector2.h"
#include "Math/Rect.h"

class Physics
{
public:

	static bool checkCollision(const Vector2& point, const Rect& rect)
	{
		return point.x > rect.pos.x &&
			point.x < rect.pos.x + rect.size.x &&
			point.y > rect.pos.y &&
			point.y < rect.pos.y + rect.size.y;
	}

	static bool checkCollision(const Rect& shape1, const Rect& shape2)
	{
		return (shape1.pos.x < shape2.pos.x + shape2.size.x &&
			shape1.pos.x + shape1.size.x > shape2.pos.x &&
			shape1.pos.y < shape2.pos.y + shape2.size.y &&
			shape1.pos.y + shape1.size.y > shape2.pos.y);
	}
};