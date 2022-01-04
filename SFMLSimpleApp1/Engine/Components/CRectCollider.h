#pragma once

#include "Math/Rect.h"
#include "Component.h"

class CRectCollider : public Component
{
public:
	Rect rect;
	bool enabled;

	CRectCollider(Rect col) : rect(col), enabled(true) { }
};