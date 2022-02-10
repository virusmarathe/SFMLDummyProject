#include "Primitives.h"

EntityManager * Primitives::_entities = NULL;

std::shared_ptr<Entity> Primitives::DrawLine(Vector2 start, Vector2 end, sf::Color color)
{
	auto e1 = _entities->addEntity("PrimitiveLine");
	e1->addComponent<CShapeLine>(start, end, color);

	return e1;
}
