#pragma once

#include "Vector2.h"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"

struct Rect
{
	Vector2 pos;
	Vector2 size;

	Rect() {}
	Rect(const sf::FloatRect& rect) : pos((float)rect.left, (float)rect.top), size((float)rect.width, (float)rect.height) { }
	Rect(const sf::IntRect& rect) : pos((float)rect.left, (float)rect.top), size((float)rect.width, (float)rect.height) { }
	Rect(Vector2 pos, Vector2 size) : pos(pos), size(size) { }
	Rect(float x, float y, float w, float h) : pos(x, y), size(w, h) { }

	Vector2 center() { return pos + (size / 2); }

	bool operator>(const Rect& rhs) const { return (size.x * size.y) > (rhs.size.x * rhs.size.y); }
	bool operator<(const Rect& rhs) const { return (size.x * size.y) < (rhs.size.x * rhs.size.y); }
	bool operator==(const Rect& rhs) const { return pos == rhs.pos && size == rhs.size; }


	friend sf::Packet& operator<< (sf::Packet& packet, const Rect& rect) { return packet << rect.pos << rect.size; }
	friend sf::Packet& operator>> (sf::Packet& packet, Rect& rect) { return packet >> rect.pos >> rect.size; }
};