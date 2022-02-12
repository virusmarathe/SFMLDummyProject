#include "Primitives.h"

EntityManager * Primitives::_entities = NULL;

std::shared_ptr<Entity> Primitives::DrawLine(Vector2 start, Vector2 end, sf::Color color)
{
	auto e1 = _entities->addEntity("PrimitiveLine");
	e1->addComponent<CShapeLine>(start, end, color);

	return e1;
}

std::shared_ptr<Entity> Primitives::DrawRectShape(Rect rect, sf::Color color)
{
	auto e1 = _entities->addEntity("PrimitiveRect");
	e1->addComponent<CTransform>(rect.pos);
	e1->addComponent<CShapeRect>(rect, color);

	return e1;
}

std::shared_ptr<Entity> Primitives::TiledSprite(Rect size, float tileSize, sf::Texture& texture, std::string tag)
{
	auto e1 = _entities->addEntity("tag");
	e1->addComponent<CTransform>(size.pos);
	std::shared_ptr<CSprite> spriteComp = e1->addComponent<CSprite>(texture);
	sf::IntRect texRect = spriteComp->sprite.getTextureRect();
	Vector2 scale(tileSize / texRect.width, tileSize / texRect.height);
	spriteComp->sprite.setScale(scale.x, scale.y);
	spriteComp->sprite.setTextureRect(sf::IntRect(0, 0, (int)(size.size.x / scale.x), (int)(size.size.y / scale.y)));

	return e1;
}
