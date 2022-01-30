#pragma once

#include "SFML/Graphics.hpp"
#include "Component.h"
#include "Math/Vector2.h"

class CShapeLine : public Component
{
public:
	sf::Vector2f start;
	sf::Vector2f end;

	CShapeLine(Vector2 startPos, Vector2 endPos) : start(sf::Vector2f(startPos.x, startPos.y)), end(sf::Vector2f(endPos.x, endPos.y)) { }
};