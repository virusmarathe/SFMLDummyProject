#include "Primitives.h"

EntityManager * Primitives::_entities = NULL;

Entity Primitives::DrawLine(Vector2 start, Vector2 end, sf::Color color)
{
	auto e1 = _entities->addEntity("PrimitiveLine");
	auto& comp = e1.addComponent<CShapeLine>();
	comp.start = sf::Vector2f(start.x, start.y);
	comp.end = sf::Vector2f(end.x, end.y);
	comp.color = color;
	return e1;
}

Entity Primitives::DrawRectShape(Rect rect, sf::Color color)
{
	auto e1 = _entities->addEntity("PrimitiveRect");
	auto& transform = e1.addComponent<CTransform>();
	transform.position = rect.pos;
	auto& rectShape = e1.addComponent<CShapeRect>();
	rectShape.rectShape = sf::RectangleShape(sf::Vector2f(rect.size.x, rect.size.y));
	rectShape.color = color;
	return e1;
}

Entity Primitives::TiledSprite(Rect size, float tileSize, sf::Texture& texture, std::string tag)
{
	auto e1 = _entities->addEntity(tag);
	auto& transform = e1.addComponent<CTransform>();
	transform.position = size.pos;
	auto& spriteComp = e1.addComponent<CSprite>();
	spriteComp.sprite = sf::Sprite(texture);
	sf::IntRect texRect = spriteComp.sprite.getTextureRect();
	Vector2 scale(tileSize / texRect.width, tileSize / texRect.height);
	spriteComp.sprite.setScale(scale.x, scale.y);
	spriteComp.sprite.setTextureRect(sf::IntRect(0, 0, (int)(size.size.x / scale.x), (int)(size.size.y / scale.y)));
	return e1;
}

Entity Primitives::ScaledSprite(Rect rect, sf::Texture& texture, bool autoCollider, std::string tag)
{
	auto e1 = _entities->addEntity(tag);
	auto& transform = e1.addComponent<CTransform>();
	transform.position = rect.pos;
	auto& spriteComp = e1.addComponent<CSprite>();
	spriteComp.sprite = sf::Sprite(texture);
	sf::IntRect texRect = spriteComp.sprite.getTextureRect();

	Vector2 scale(rect.size.x / ((float)texRect.width), rect.size.y / ((float)texRect.height));
	spriteComp.sprite.setScale(scale.x, scale.y);

	if (autoCollider)
	{
		auto& rectCollider = e1.addComponent<CRectCollider>();
		rectCollider.rect = rect;
	}

	return e1;
}

Entity Primitives::DrawText(const char* text, Vector2 pos, sf::Font& font, unsigned int fontSize, sf::Color color)
{
	auto ent = _entities->addEntity("PrimitiveText");
	auto& transform = ent.addComponent<CTransform>();
	transform.position = pos;
	auto& textComp = ent.addComponent<CText>();
	textComp.text = sf::Text(sf::String(text), font, fontSize);
	textComp.textColor = color;

	return ent;
}
