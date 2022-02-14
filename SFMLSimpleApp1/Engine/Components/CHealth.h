#pragma once
#include "Component.h"

class CHealth : public Component
{
public:
	float health;
	int hitLayer;

	CHealth(float health, int layer) : health(health), hitLayer(layer) { }
};