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
        sf::Vector2i pixelPos = sf::Mouse::getPosition(*_window);
        Vector2 mousePos = (*_window).mapPixelToCoords(pixelPos);

        // temporary camera movement
        for (auto ent : _entities->getEntities("Camera"))
        {
            auto transform = ent->getComponent<CTransform>();
            Vector2 startPos = transform->position - transform->scale / 2.0f;
            sf::View view(sf::FloatRect(startPos.x, startPos.y, transform->scale.x, transform->scale.y));
            _window->setView(view);
        }

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

            if (ent->hasComponent<CShapeRect>())
            {
                sf::RectangleShape shape = ent->getComponent<CShapeRect>()->rectShape;
                shape.setPosition(sf::Vector2f(ent->getComponent<CTransform>()->position.x, ent->getComponent<CTransform>()->position.y));
                _window->draw(shape);
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