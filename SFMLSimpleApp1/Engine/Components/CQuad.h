#pragma once
#include "Math/Rect.h"
#include "Component.h"

struct CQuad : public Component
{
	Rect rect;
	Rect texCoords;
	unsigned char textureID = 1;
};