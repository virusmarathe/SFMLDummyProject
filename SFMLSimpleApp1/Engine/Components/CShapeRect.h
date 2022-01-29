#pragma once

#include "SFML/Graphics.hpp"
#include "Component.h"
#include "Math/Rect.h"

class CShapeRect : public Component
{
public:
	sf::RectangleShape rectShape;

	CShapeRect(Rect rectSize) : rectShape(sf::Vector2f(rectSize.size.x, rectSize.size.y)) { }
};