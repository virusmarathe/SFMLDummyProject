#pragma once

#include "SFML/Graphics.hpp"
#include "Component.h"
#include "Math/Rect.h"

class CShapeRect : public Component
{
public:
	sf::RectangleShape rectShape;
	sf::Color color;

	CShapeRect(Rect rectSize) : rectShape(sf::Vector2f(rectSize.size.x, rectSize.size.y)), color(sf::Color::White) { }
	CShapeRect(Rect rectSize, sf::Color color) : rectShape(sf::Vector2f(rectSize.size.x, rectSize.size.y)), color(color) { }
};