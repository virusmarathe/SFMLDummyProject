#pragma once
#include "Component.h"
#include "SFML/Graphics.hpp"

class CHealth : public Component
{
public:
	float health = 0;
	int hitLayer = 0;
	float lastHitTimer = 10;
	float maxHealth = 0;
	sf::RectangleShape background;
	sf::RectangleShape foreground;
	bool rendered = false;

	CHealth() {}

	CHealth(float health, int layer) : health(health), hitLayer(layer), lastHitTimer(10), maxHealth(health)
	{
		background.setSize(sf::Vector2f(100, 10));
		background.setFillColor(sf::Color::Red);
		foreground.setSize(sf::Vector2f(100, 10));
		foreground.setFillColor(sf::Color::Green);
	}
};