#pragma once

#include "SFML/Graphics.hpp"

class CText
{
public:
	sf::Text text;
	sf::Color textColor;

	CText(const char * textString, sf::Font& font, unsigned int fontSize, sf::Color color) : text(sf::String(textString), font, fontSize)
	{
		text.setFillColor(color);
	}
};