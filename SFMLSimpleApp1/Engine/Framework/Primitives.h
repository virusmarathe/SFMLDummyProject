#pragma once

#include "Entity/EntityManager.h"
#include "Math/Vector2.h"

class Primitives
{
public:

	static std::shared_ptr<Entity> DrawLine(Vector2 start, Vector2 end, sf::Color color = sf::Color::White);
    static EntityManager* _entities;
};

