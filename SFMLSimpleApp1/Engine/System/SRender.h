#pragma once

#include "System.h"
#include "Physics/Physics.h"

class SRender : public System
{
public:

    SRender(EntityManager* entityManagerRef, int priority, sf::RenderWindow * windowRef) : _window(windowRef), System(entityManagerRef, priority) { }

	virtual void update(float dt) override
	{
        _window->clear();
        Vector2 mousePos = sf::Mouse::getPosition(*_window);

        for (auto ent : _entities->getEntities())
        {
            if (ent->hasComponent<CSprite>())
            {
                ent->getComponent<CSprite>()->sprite.setPosition(ent->getComponent<CTransform>()->position.x, ent->getComponent<CTransform>()->position.y);
                _window->draw(ent->getComponent<CSprite>()->sprite);
            }

            if (ent->hasComponent<CText>())
            {
                ent->getComponent<CText>()->text.setPosition(ent->getComponent<CTransform>()->position.x, ent->getComponent<CTransform>()->position.y);
                _window->draw(ent->getComponent<CText>()->text);
            }

            if (GameEngine::DEBUG_MODE)
            {
                if (ent->hasComponent<CRectCollider>())
                {
                    Rect rect = ent->getComponent<CRectCollider>()->rect;
                    sf::Vertex line[] =
                    {
                        sf::Vertex(sf::Vector2f(rect.pos.x, rect.pos.y)),
                        sf::Vertex(sf::Vector2f(rect.pos.x + rect.size.x, rect.pos.y)),
                        sf::Vertex(sf::Vector2f(rect.pos.x + rect.size.x, rect.pos.y + rect.size.y)),
                        sf::Vertex(sf::Vector2f(rect.pos.x, rect.pos.y + rect.size.y)),
                        sf::Vertex(sf::Vector2f(rect.pos.x, rect.pos.y))
                    };
                    line[0].color = line[1].color = line[2].color = line[3].color = line[4].color = sf::Color::Green;
                    if (Physics::checkCollision(mousePos, rect))
                    {
                        line[0].color = line[1].color = line[2].color = line[3].color = line[4].color = sf::Color::Red;
                    }

                    _window->draw(line, 5, sf::LineStrip);
                }
            }
        }

        _window->display();
	}

private:
    sf::RenderWindow* _window = nullptr;
};