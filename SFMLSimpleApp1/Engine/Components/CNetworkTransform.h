#pragma once
#include "Math/Vector2.h"
#include "Component.h"

class CNetworkTransform : public Component
{
public:
	Vector2 position;
	Vector2 velocity;

	CNetworkTransform() {}
	CNetworkTransform(Vector2 pos) : position(pos) { }
};