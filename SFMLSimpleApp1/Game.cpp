#include "Game.h"
#include "Paddle.h"
#include "Ball.h"
#include "Scoreboard.h"
#include <iostream>
#include "Config.h"
#include "Engine/Math/Vector2.h"
#include "Engine/Entity.h"

void Game::init(sf::RenderWindow * window)
{
    loadResources();

    _window = window;

    _ball = new Ball(Vector2(_window->getSize().x / 2.0f, _window->getSize().y / 2.0f), BALL_SIZE, _window, _ballTexture);
    _player1Paddle = new Paddle(sf::Color::Red, Vector2(_window->getSize().x - PADDLE_WIDTH, 0), _window);
    _player2Paddle = new Paddle(sf::Color::Blue, Vector2(), _window);
    _scoreboard = new Scoreboard(_font, 24, _window);

    _ballEntity = std::make_shared<Entity>(0);
    _ballEntity->transform = std::make_shared<CTransform>(Vector2(_window->getSize().x / 2.0f, _window->getSize().y / 2.0f),
                                                          Vector2((BALL_SIZE * 2) / _ballTexture.getSize().x, (BALL_SIZE * 2) / _ballTexture.getSize().y),
                                                          Vector2(0,200));
    _ballEntity->renderer = std::make_shared<CSpriteRenderer>(_ballTexture);
    _ballEntity->renderer->sprite.setScale(_ballEntity->transform->scale.x, _ballEntity->transform->scale.y);
    _ballEntity->collider = std::make_shared<CRectCollider>(sf::FloatRect(_ballEntity->transform->position.x, _ballEntity->transform->position.y, BALL_SIZE * 2, BALL_SIZE * 2));

    _entities.push_back(_ballEntity);

    std::shared_ptr<Entity> topWall = std::make_shared<Entity>(1);
    topWall->transform = std::make_shared<CTransform>(Vector2(0, 0));
    topWall->collider = std::make_shared<CRectCollider>(sf::FloatRect(0,0,_window->getSize().x, 50));

    std::shared_ptr<Entity> bottomWall = std::make_shared<Entity>(2);
    bottomWall->transform = std::make_shared<CTransform>(Vector2(0, _window->getSize().y - 50));
    bottomWall->collider = std::make_shared<CRectCollider>(sf::FloatRect(0, _window->getSize().y - 50, _window->getSize().x, 50));

    _entities.push_back(topWall);
    _entities.push_back(bottomWall);

    _player1Entity = std::make_shared<Entity>(3);
    _player1Entity->transform = std::make_shared<CTransform>(Vector2(0, 0));
    _player1Entity->renderer = std::make_shared<CSpriteRenderer>(_paddleTexture);
    _player1Entity->collider = std::make_shared<CRectCollider>(sf::FloatRect(_player1Entity->renderer->sprite.getTextureRect()));
    _entities.push_back(_player1Entity);

    _player2Entity = std::make_shared<Entity>(4);
    _player2Entity->transform = std::make_shared<CTransform>(Vector2(_window->getSize().x - PADDLE_WIDTH, 0));
    _player2Entity->renderer = std::make_shared<CSpriteRenderer>(_paddleTexture);
    _player2Entity->collider = std::make_shared<CRectCollider>(sf::FloatRect(_player2Entity->renderer->sprite.getTextureRect()));
    _entities.push_back(_player2Entity);

    _gameObjects.push_back(_ball);
    _gameObjects.push_back(_player1Paddle);
    _gameObjects.push_back(_player2Paddle);
    _gameObjects.push_back(_scoreboard);
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

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))    _player1Paddle->SetVelocity(1);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))      _player1Paddle->SetVelocity(-1);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))       _player2Paddle->SetVelocity(-1);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))       _player2Paddle->SetVelocity(1);
}

void Game::update(float dt)
{
    for (int i = 0; i < _gameObjects.size(); i++)
    {
        _gameObjects[i]->update(dt);
    }

    // trying out a movement system
    for (auto ent : _entities)
    {
        if (ent->transform)
        {
            ent->transform->position += ent->transform->velocity * dt;
            ent->collider->collider.left = ent->transform->position.x;
            ent->collider->collider.top = ent->transform->position.y;
        }
    }

    for (auto ent : _entities)
    {
        if (ent != _ballEntity && hasCollision(_ballEntity->collider->collider, ent->collider->collider))
        {
            _ballEntity->transform->velocity *= -1;
        }
    }

    if (hasCollision(_ball->getCollisionRect(), _player1Paddle->getCollisionRect()) || hasCollision(_ball->getCollisionRect(), _player2Paddle->getCollisionRect()))
    {
        _ball->reverse();
    }
}

void Game::render()
{
    _window->clear();

    for (int i = 0; i < _gameObjects.size(); i++)
    {
        _gameObjects[i]->render();
    }

    // trying out a rendering system
    for (auto ent : _entities)
    {
        if (ent->renderer)
        {
            ent->renderer->sprite.setPosition(ent->transform->position.x, ent->transform->position.y);
            _window->draw(ent->renderer->sprite);
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
    _scoreboard->playerScored(playerNum);
}

Game::~Game()
{
    for (int i = 0; i < _gameObjects.size(); i++)
    {
        delete _gameObjects[i];
    }
    _gameObjects.clear();
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
