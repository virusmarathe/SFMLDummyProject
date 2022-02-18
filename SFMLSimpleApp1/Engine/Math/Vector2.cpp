#include "Vector2.h"

sf::Packet& operator<<(sf::Packet& packet, const Vector2& vec)
{
	return packet << vec.x << vec.y;
}

sf::Packet& operator>>(sf::Packet& packet, Vector2& vec)
{
	return packet >> vec.x >> vec.y;
}
