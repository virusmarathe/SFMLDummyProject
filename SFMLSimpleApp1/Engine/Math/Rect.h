#pragma once

#include "Vector2.h"
#include "SFML/Graphics.hpp"

struct Rect
{
	Vector2 pos;
	Vector2 size;

	Rect(const sf::FloatRect& rect) : pos((float)rect.left, (float)rect.top), size((float)rect.width, (float)rect.height) { }
	Rect(const sf::IntRect& rect) : pos((float)rect.left, (float)rect.top), size((float)rect.width, (float)rect.height) { }
	Rect(Vector2 pos, Vector2 size) : pos(pos), size(size) { }
	Rect(float x, float y, float w, float h) : pos(x, y), size(w, h) { }
};