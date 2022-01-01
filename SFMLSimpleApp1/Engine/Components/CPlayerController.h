#pragma once

#include "SFML/Graphics.hpp"
#include "Component.h"

class CPlayerController : public Component
{
public:
	sf::Keyboard::Key upKey;
	sf::Keyboard::Key downKey;
	sf::Keyboard::Key leftKey;
	sf::Keyboard::Key rightKey;

	CPlayerController(sf::Keyboard::Key up, sf::Keyboard::Key down, sf::Keyboard::Key left, sf::Keyboard::Key right) :
		upKey(up), downKey(down), leftKey(left), rightKey(right) { }
};