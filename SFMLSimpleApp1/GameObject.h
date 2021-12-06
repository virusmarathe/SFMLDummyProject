#pragma once
#include "SFML/Graphics.hpp"

class GameObject
{
public:
	virtual void update(float dt) { };
	virtual void render() { };
	sf::RenderWindow * _window;
};

