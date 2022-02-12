#pragma once

#include "Math/Rect.h"
#include "Component.h"

class CRectCollider : public Component
{
public:
	Rect rect;
	Vector2 offset;
	bool enabled;
	bool isTrigger;

	CRectCollider(Rect col) : rect(col), enabled(true), offset(0,0), isTrigger(false) { }
	CRectCollider(Rect col, Vector2 off) : rect(col), enabled(true), offset(off), isTrigger(false) { }
	CRectCollider(Rect col, bool trigger) : rect(col), enabled(true), offset(0, 0), isTrigger(trigger) { }
	CRectCollider(Rect col, Vector2 off, bool trigger) : rect(col), enabled(true), offset(off), isTrigger(trigger) { }
};