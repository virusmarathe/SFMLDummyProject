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

	static bool checkCollision(const Vector2& origin, const Vector2& dir, const Rect& rect, Vector2& contactPoint, Vector2& normal, float& tHitNear)
	{
		Vector2 topLeft = rect.pos;
		Vector2 bottomRight = topLeft + rect.size;

		float tx1 = (topLeft.x - origin.x) / dir.x;
		float tx2 = (bottomRight.x - origin.x) / dir.x;
		float ty1 = (topLeft.y - origin.y) / dir.y;
		float ty2 = (bottomRight.y - origin.y) / dir.y;

		bool closeCheck = tx1 < tx2;
		float nearX = closeCheck ? tx1 : tx2;
		float farX = closeCheck ? tx2 : tx1;
		closeCheck = ty1 < ty2;
		float nearY = closeCheck ? ty1 : ty2;
		float farY = closeCheck ? ty2 : ty1;

		tHitNear = std::max(nearX, nearY);
		contactPoint = origin + (dir * tHitNear);
		int xSign = dir.x > 0 ? -1 : 1;
		int ySign = dir.y > 0 ? -1 : 1;
		normal = nearX > nearY ? Vector2((float)xSign, 0) : Vector2(0, (float)ySign);

		float tFar = std::min(farX, farY);

		if (tFar < 0) return false;
		if (tHitNear > 1) return false;

		return nearX < farY && nearY < farX;
	}
};