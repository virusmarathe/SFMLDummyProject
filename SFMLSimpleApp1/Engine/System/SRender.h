#pragma once

#include "System.h"
#include "Physics/Physics.h"
#include "Resources/Assets.h"

class SRender : public System
{
public:

    SRender(EntityManager* entityManagerRef, int priority, sf::RenderWindow * windowRef, Entity camera, std::shared_ptr<Assets> assetsRef) : _window(windowRef), _camera(camera), _assets(assetsRef), System(entityManagerRef, priority) { }

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

        EntityList allQuads = _entities->getEntitiesByType<CQuad>();
        int counter = 0;
        std::map<unsigned char, EntityList> textureToQuadsMap; // should probably be switched to array

        for (auto ent : allQuads)
        {
            textureToQuadsMap[ent.getComponent<CQuad>().textureID].push_back(ent);
        }

        for (auto const& pair : textureToQuadsMap)
        {
            counter = 0;
            sf::VertexArray quadVertices(sf::Quads, pair.second.size() * 4);
            for (auto ent : pair.second)
            {
                Vector2 pos = ent.getComponent<CTransform>().position;
                Rect sizeRect = ent.getComponent<CQuad>().rect;
                Rect texRect = ent.getComponent<CQuad>().texCoords;
                quadVertices[counter + 0].position = sf::Vector2f(pos.x, pos.y);
                quadVertices[counter + 0].texCoords = sf::Vector2f(0, 0);
                quadVertices[counter + 1].position = sf::Vector2f(pos.x + sizeRect.size.x, pos.y);
                quadVertices[counter + 1].texCoords = sf::Vector2f(texRect.size.x, 0);
                quadVertices[counter + 2].position = sf::Vector2f(pos.x + sizeRect.size.x, pos.y + sizeRect.size.y);
                quadVertices[counter + 2].texCoords = sf::Vector2f(texRect.size.x, texRect.size.y);
                quadVertices[counter + 3].position = sf::Vector2f(pos.x, pos.y + sizeRect.size.y);
                quadVertices[counter + 3].texCoords = sf::Vector2f(0, texRect.size.y);
                counter += 4;
            }

            sf::RenderStates states(&_assets->getTextureFromID(pair.first));
            _window->draw(quadVertices, states);
        }

        if (GameEngine::DEBUG_MODE)
        {
            EntityList colliders = _entities->getEntitiesByType<CRectCollider>();
            counter = 0;
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
    std::shared_ptr<Assets> _assets;
};