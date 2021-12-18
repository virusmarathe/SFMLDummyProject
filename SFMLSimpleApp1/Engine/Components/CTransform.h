#pragma once
#include "Math/Vector2.h"

class CTransform
{
public:
	Vector2 position;
	Vector2 scale;

	CTransform(Vector2 pos) : position(pos), scale(Vector2(1,1)) { }
	CTransform(Vector2 pos, Vector2 scale) : position(pos), scale(scale) { }
};