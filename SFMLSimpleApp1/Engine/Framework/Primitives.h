#pragma once

#include "Entity/EntityManager.h"
#include "Resources/Assets.h"
#include "Math/Vector2.h"

class Primitives
{
public:

	static Entity DrawLine(Vector2 start, Vector2 end, sf::Color color = sf::Color::White);
	static Entity DrawRectShape(Rect rect, sf::Color color = sf::Color::White);
	static Entity TiledSprite(Rect size, float tileSize, sf::Texture& texture, std::string tag = "PrimitiveTiledSprite");
	static Entity ScaledSprite(Rect rect, sf::Texture& texture, bool autoCollider = false, std::string tag = "PrimitiveScaledSprite");
	static Entity DrawText(const char* text, Vector2 pos, sf::Font& font, unsigned int fontSize = 24, sf::Color color = sf::Color::White);
    static EntityManager* _entities;
};

