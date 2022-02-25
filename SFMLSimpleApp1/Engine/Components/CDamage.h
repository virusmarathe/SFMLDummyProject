#pragma once
#include "Component.h"

class CDamage : public Component
{
public:
	float damage = 0;
	int layer = 0;

	CDamage() {}
	CDamage(float damage, int layer) : damage(damage), layer(layer) { }
};