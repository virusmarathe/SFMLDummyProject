#pragma once
#include "SFML/Graphics.hpp"

class CSpriteRenderer
{
public:
	sf::Sprite sprite;

	CSpriteRenderer(sf::Texture& texture) { sprite.setTexture(texture); }
};