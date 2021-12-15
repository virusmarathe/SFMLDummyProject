#pragma once
#include "SFML/Graphics.hpp"

class CSprite
{
public:
	sf::Sprite sprite;

	CSprite(sf::Texture& texture) : sprite(texture) { }
};