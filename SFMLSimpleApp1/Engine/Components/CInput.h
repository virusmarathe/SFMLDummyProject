#pragma once

#include "SFML/Graphics.hpp"
#include "Component.h"

class CInput : public Component
{
public:
	bool up;
	bool down;
	bool left;
	bool right;

	CInput() :up(false), down(false), left(false), right(false) {}
};