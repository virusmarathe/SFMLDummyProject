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
