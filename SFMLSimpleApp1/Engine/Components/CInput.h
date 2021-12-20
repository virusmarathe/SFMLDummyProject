#pragma once

#include "SFML/Graphics.hpp"

class CInput
{
public:
	bool up;
	bool down;
	bool left;
	bool right;

	CInput() :up(false), down(false), left(false), right(false) {}
};