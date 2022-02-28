#pragma once

#include "System.h"
#include "Physics/Physics.h"

class SRender : public System
{
public:

    SRender(EntityManager* entityManagerRef, int priority, sf::RenderWindow * windowRef, Entity camera) : _window(windowRef), _camera(camera), System(entityManagerRef, priority) { }

	virtual void update(float dt) override
	{
        _window->clear();
        sf::Vector2i pixelPos = sf::Mouse::getPosition(*_window);
        Vector2 mousePos = (*_window).mapPixelToCoords(pixelPos);

        // temporary camera movement
        auto& transform = _camera.getComponent<CTransform>();
        Vector2 startPos = transform.position - transform.scale / 2.0f;
        Rect viewPort(startPos.x, startPos.y, transform.scale.x, transform.scale.y);
        sf::View view(sf::FloatRect(startPos.x, startPos.y, transform.scale.x, transform.scale.y));
        _window->setView(view);


        for (auto ent : _entities->getEntitiesByType<CSprite>())
        {
            Vector2 pos = ent.getComponent<CTransform>().position;
            Rect spriteRect = Rect(pos.x, pos.y, ent.getComponent<CSprite>().sprite.getLocalBounds().width, ent.getComponent<CSprite>().sprite.getLocalBounds().height);
            if (!Physics::checkCollision(spriteRect, viewPort)) continue;
            ent.getComponent<CSprite>().sprite.setPosition(pos.x, pos.y);
            _window->draw(ent.getComponent<CSprite>().sprite);
        }

        for (auto ent : _entities->getEntities())
        {
            if (ent.hasComponent<CShapeRect>())
            {
                sf::RectangleShape shape = ent.getComponent<CShapeRect>().rectShape;
                shape.setFillColor(ent.getComponent<CShapeRect>().color);
                shape.setPosition(sf::Vector2f(ent.getComponent<CTransform>().position.x, ent.getComponent<CTransform>().position.y));
                _window->draw(shape);
            }

            if (ent.hasComponent<CShapeLine>())
            {
                sf::Vector2f start(ent.getComponent<CShapeLine>().start.x, ent.getComponent<CShapeLine>().start.y);
                sf::Vector2f end(ent.getComponent<CShapeLine>().end.x, ent.getComponent<CShapeLine>().end.y);
                sf::Vertex line[] = { sf::Vertex(start), sf::Vertex(end) };
                line[0].color = line[1].color = ent.getComponent<CShapeLine>().color;
                _window->draw(line, 2, sf::Lines);
            }
           
            if (ent.hasComponent<CHealth>())
            {
                if (ent.getComponent<CHealth>().rendered)
                {
                    Vector2 barPosition = ent.getComponent<CTransform>().position + Vector2(0, -50);
                    ent.getComponent<CHealth>().background.setPosition(sf::Vector2f(barPosition.x, barPosition.y));
                    ent.getComponent<CHealth>().foreground.setPosition(sf::Vector2f(barPosition.x, barPosition.y));
                    _window->draw(ent.getComponent<CHealth>().background);
                    _window->draw(ent.getComponent<CHealth>().foreground);
                }
            }            
        }

        if (GameEngine::DEBUG_MODE)
        {
            EntityList colliders = _entities->getEntitiesByType<CRectCollider>();
            int counter = 0;
            sf::VertexArray vertices(sf::PrimitiveType::Lines, colliders.size() * 8);
            for (auto ent : colliders)
            {
                Rect rect = ent.getComponent<CRectCollider>().rect;
                vertices[counter++] = sf::Vertex(sf::Vector2f(rect.pos.x, rect.pos.y), sf::Color::Green);
                vertices[counter++] = sf::Vertex(sf::Vector2f(rect.pos.x + rect.size.x, rect.pos.y), sf::Color::Green);
                vertices[counter++] = sf::Vertex(sf::Vector2f(rect.pos.x + rect.size.x, rect.pos.y), sf::Color::Green);
                vertices[counter++] = sf::Vertex(sf::Vector2f(rect.pos.x + rect.size.x, rect.pos.y + rect.size.y), sf::Color::Green);
                vertices[counter++] = sf::Vertex(sf::Vector2f(rect.pos.x + rect.size.x, rect.pos.y + rect.size.y), sf::Color::Green);
                vertices[counter++] = sf::Vertex(sf::Vector2f(rect.pos.x, rect.pos.y + rect.size.y), sf::Color::Green);
                vertices[counter++] = sf::Vertex(sf::Vector2f(rect.pos.x, rect.pos.y + rect.size.y), sf::Color::Green);
                vertices[counter++] = sf::Vertex(sf::Vector2f(rect.pos.x, rect.pos.y), sf::Color::Green);
            }
            _window->draw(vertices);
        }

        for (auto ent : _entities->getEntitiesByType<CText>())
        {
            ent.getComponent<CText>().text.setPosition(ent.getComponent<CTransform>().position.x, ent.getComponent<CTransform>().position.y);
            ent.getComponent<CText>().text.setFillColor(ent.getComponent<CText>().textColor);
            _window->draw(ent.getComponent<CText>().text);
        }

        _window->display();
	}

private:
    sf::RenderWindow* _window = nullptr;
    Entity _camera;
};