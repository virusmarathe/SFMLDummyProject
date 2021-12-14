#pragma once

#include "SFML/Graphics.hpp"

class CText
{
public:
	sf::Text text;
	unsigned int fontSize;
	sf::Color textColor;

	CText(sf::Font& font, unsigned int fontSize, sf::Color color) : fontSize(fontSize) 
	{
		text.setFont(font);
		text.setCharacterSize(fontSize);
		text.setFillColor(color);
	}
};