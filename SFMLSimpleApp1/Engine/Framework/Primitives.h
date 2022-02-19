#pragma once

#include "Entity/EntityManager.h"
#include "Resources/Assets.h"
#include "Math/Vector2.h"

class Primitives
{
public:

	static std::shared_ptr<Entity> DrawLine(Vector2 start, Vector2 end, sf::Color color = sf::Color::White);
	static std::shared_ptr<Entity> DrawRectShape(Rect rect, sf::Color color = sf::Color::White);
	static std::shared_ptr<Entity> TiledSprite(Rect size, float tileSize, sf::Texture& texture, std::string tag = "PrimitiveTiledSprite");
	static std::shared_ptr<Entity> ScaledSprite(Rect rect, sf::Texture& texture, bool autoCollider = false, std::string tag = "PrimitiveScaledSprite");
	static std::shared_ptr<Entity> DrawText(const char* text, Vector2 pos, sf::Font& font, float fontSize = 24, sf::Color color = sf::Color::White);
    static EntityManager* _entities;
};

