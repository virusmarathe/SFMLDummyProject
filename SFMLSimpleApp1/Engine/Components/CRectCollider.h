#pragma once

#include "Math/Rect.h"
#include "Component.h"

class CRectCollider : public Component
{
public:
	Rect rect;
	Vector2 offset;
	bool enabled;

	CRectCollider(Rect col) : rect(col), enabled(true), offset(0,0) { }
	CRectCollider(Rect col, Vector2 off) : rect(col), enabled(true), offset(off) { }
};