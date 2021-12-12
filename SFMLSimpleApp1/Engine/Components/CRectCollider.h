#pragma once

#include "SFML/Graphics.hpp"

class CRectCollider
{
public:
	sf::FloatRect collider;
	bool enabled;

	CRectCollider(sf::FloatRect col) : collider(col), enabled(true) { }
};