#pragma once

#include "Math/Rect.h"

class CRectCollider
{
public:
	Rect rect;
	bool enabled;

	CRectCollider(Rect col) : rect(col), enabled(true) { }
};