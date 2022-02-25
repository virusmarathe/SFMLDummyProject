#pragma once
#include "SFML/Graphics.hpp"
#include "Component.h"

class CSprite : public Component
{
public:
	sf::Sprite sprite;

	CSprite() {}
	CSprite(sf::Texture& texture) : sprite(texture) { }
};