#pragma once
#include <string>
#include <SFML/Graphics.hpp>

struct Animation
{
	Animation(sf::Texture& texture, int numFrames, float duration) : 
		texture(texture),
		numFrames(numFrames),
		frameWidth(texture.getSize().x / numFrames),
		frameHeight(texture.getSize().y),
		duration(duration) { }

	sf::Texture& texture;
	int frameWidth;
	int frameHeight;
	int numFrames;
	float duration;
};