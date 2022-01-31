#pragma once

#include "SFML/Graphics.hpp"
#include "Component.h"
#include "Math/Vector2.h"

class CShapeLine : public Component
{
public:
	sf::Vector2f start;
	sf::Vector2f end;
	sf::Color color;

	CShapeLine(Vector2 startPos, Vector2 endPos) : start(sf::Vector2f(startPos.x, startPos.y)), end(sf::Vector2f(endPos.x, endPos.y)), color(sf::Color::White){ }
	CShapeLine(Vector2 startPos, Vector2 endPos, sf::Color c) : start(sf::Vector2f(startPos.x, startPos.y)), end(sf::Vector2f(endPos.x, endPos.y)), color(c) { }
};