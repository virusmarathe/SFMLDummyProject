#include "Game.h"
#include <iostream>
#include "Config.h"
#include "Engine/Math/Vector2.h"
#include "Engine/Entity.h"

void Game::init(sf::RenderWindow * window)
{
    loadResources();

    _window = window;

    _ballEntity = std::make_shared<Entity>(0);
    _ballEntity->transform = std::make_shared<CTransform>(Vector2(_window->getSize().x / 2.0f, _window->getSize().y / 2.0f),
                                                          Vector2((BALL_SIZE * 2) / _ballTexture.getSize().x, (BALL_SIZE * 2) / _ballTexture.getSize().y),
                                                          Vector2(BALL_START_SPEED,BALL_START_SPEED));
    _ballEntity->sprite = std::make_shared<CSprite>(_ballTexture);
    _ballEntity->sprite->sprite.setScale(_ballEntity->transform->scale.x, _ballEntity->transform->scale.y);
    _ballEntity->collider = std::make_shared<CRectCollider>(sf::FloatRect(_ballEntity->transform->position.x, _ballEntity->transform->position.y, BALL_SIZE * 2, BALL_SIZE * 2));

    _entities.push_back(_ballEntity);

    std::shared_ptr<Entity> topWall = std::make_shared<Entity>(1);
    topWall->transform = std::make_shared<CTransform>(Vector2(0, 0));
    topWall->collider = std::make_shared<CRectCollider>(sf::FloatRect(0,0,(float)_window->getSize().x, 50));

    std::shared_ptr<Entity> bottomWall = std::make_shared<Entity>(2);
    bottomWall->transform = std::make_shared<CTransform>(Vector2(0, (float)_window->getSize().y - 50));
    bottomWall->collider = std::make_shared<CRectCollider>(sf::FloatRect(0, (float)_window->getSize().y - 50, (float)_window->getSize().x, 50));

    _entities.push_back(topWall);
    _entities.push_back(bottomWall);

    _player1Entity = std::make_shared<Entity>(3);
    _player1Entity->transform = std::make_shared<CTransform>(Vector2(500, 200));
    _player1Entity->sprite = std::make_shared<CSprite>(_paddleTexture);
    _player1Entity->collider = std::make_shared<CRectCollider>(sf::FloatRect(_player1Entity->sprite->sprite.getTextureRect()));
    _entities.push_back(_player1Entity);

    _player2Entity = std::make_shared<Entity>(4);
    _player2Entity->transform = std::make_shared<CTransform>(Vector2((float)_window->getSize().x - PADDLE_WIDTH, 200));
    _player2Entity->sprite = std::make_shared<CSprite>(_paddleTexture);
    _player2Entity->collider = std::make_shared<CRectCollider>(sf::FloatRect(_player2Entity->sprite->sprite.getTextureRect()));
    _entities.push_back(_player2Entity);

    _player1Score = std::make_shared<Entity>(5);
    _player1Score->transform = std::make_shared<CTransform>(Vector2(_window->getSize().x / 2.0f, 20));
    _player1Score->text = std::make_shared<CText>(_font, 24, sf::Color::Blue);
    _player1Score->text->text.setString("Player 1: 0");
    _entities.push_back(_player1Score);

    _player2Score = std::make_shared<Entity>(6);
    _player2Score->transform = std::make_shared<CTransform>(Vector2(_window->getSize().x / 2.0f, 60));
    _player2Score->text = std::make_shared<CText>(_font, 24, sf::Color::Red);
    _player2Score->text->text.setString("Player 2: 0");
    _entities.push_back(_player2Score);
}

void Game::handleInput()
{
    sf::Event currEvent;
    while (_window->pollEvent(currEvent))
    {
        if (currEvent.type == sf::Event::Closed)
        {
            _window->close();
        }
        if (currEvent.type == sf::Event::KeyPressed)
        {
            if (currEvent.key.code == sf::Keyboard::P) _debugToggle = !_debugToggle;
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))       _player1Entity->transform->velocity.y = -PADDLE_SPEED;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))       _player1Entity->transform->velocity.y = PADDLE_SPEED;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))    _player2Entity->transform->velocity.y = PADDLE_SPEED;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))      _player2Entity->transform->velocity.y = -PADDLE_SPEED;
}

void Game::update(float dt)
{
    // trying out a movement system
    for (auto ent : _entities)
    {
        if (ent->transform)
        {
            ent->transform->position += ent->transform->velocity * dt;
            if (ent->collider)
            {
                ent->collider->collider.left = ent->transform->position.x;
                ent->collider->collider.top = ent->transform->position.y;
            }
        }
    }

    for (auto ent : _entities)
    {
        if (ent->collider)
        {
            if (ent != _ballEntity && hasCollision(_ballEntity->collider->collider, ent->collider->collider))
            {
                _ballEntity->transform->velocity *= -1;
            }
            if (ent != _ballEntity && ent != _player1Entity && hasCollision(_player1Entity->collider->collider, ent->collider->collider))
            {
                _player1Entity->transform->velocity.y = 0;
            }
            if (ent != _ballEntity && ent != _player2Entity && hasCollision(_player2Entity->collider->collider, ent->collider->collider))
            {
                _player2Entity->transform->velocity.y = 0;
            }
        }
    }
}

void Game::render()
{
    _window->clear();

    // trying out a rendering system
    for (auto ent : _entities)
    {
        if (ent->sprite)
        {
            ent->sprite->sprite.setPosition(ent->transform->position.x, ent->transform->position.y);
            _window->draw(ent->sprite->sprite);
        }

        if (ent->text)
        {
            ent->text->text.setPosition(ent->transform->position.x, ent->transform->position.y);
            _window->draw(ent->text->text);
        }

        if (_debugToggle && ent->collider)
        {
            sf::FloatRect rect = ent->collider->collider;
            sf::Vertex line[] =
            {
                sf::Vertex(sf::Vector2f(rect.left, rect.top)),
                sf::Vertex(sf::Vector2f(rect.left + rect.width, rect.top)),
                sf::Vertex(sf::Vector2f(rect.left + rect.width, rect.top + rect.height)),
                sf::Vertex(sf::Vector2f(rect.left, rect.top + rect.height)),
                sf::Vertex(sf::Vector2f(rect.left, rect.top))
            };
            line[0].color = line[1].color = line[2].color = line[3].color = line[4].color = sf::Color::Green;
            _window->draw(line, 5, sf::LineStrip);
        }
    }

    _window->display();
}

bool Game::hasCollision(const sf::FloatRect& shape1, const sf::FloatRect& shape2)
{
    bool foundCollision = false;

    if (shape1.left < shape2.left + shape2.width && shape1.left + shape1.width > shape2.left &&
        shape1.top < shape2.top + shape2.height && shape1.top + shape1.height > shape2.top) return true;

    return foundCollision;
}

void Game::notifyBallScored(int playerNum)
{
    // TODO
}

bool Game::loadResources()
{
    if (!_font.loadFromFile("BalooBhaijaan2-VariableFont_wght.ttf"))
    {
        std::cout << "Couldn't find font BalooBhaijaan2-VariableFont_wght.ttf";
        return false;
    }

    if (!_ballTexture.loadFromFile("notaBall.png"))
    {
        std::cout << "Couldn't find image notaBall.png";
        return false;
    }

    if (!_paddleTexture.loadFromFile("paddle.png"))
    {
        std::cout << "Couldn't find image notaBall.png";
        return false;
    }

    return true;
}
