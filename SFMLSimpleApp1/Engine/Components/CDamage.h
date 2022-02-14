#pragma once
#include "Component.h"

class CDamage : public Component
{
public:
	float damage;
	int layer;

	CDamage(float damage, int layer) : damage(damage), layer(layer) { }
};