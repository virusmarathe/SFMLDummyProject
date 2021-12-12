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

    _ball = new Ball(Vector2(_window->getSize().x / 2.0f, _window->getSize().y / 2.0f), BALL_SIZE, _window, _texture);
    _player1Paddle = new Paddle(sf::Color::Red, Vector2(_window->getSize().x - PADDLE_WIDTH, 0), _window);
    _player2Paddle = new Paddle(sf::Color::Blue, Vector2(), _window);
    _scoreboard = new Scoreboard(_font, 24, _window);

    _ballEntity = std::make_shared<Entity>();
    _ballEntity->transform = std::make_shared<CTransform>(Vector2(_window->getSize().x / 2.0f, _window->getSize().y / 2.0f), Vector2(50,20));
    _ballEntity->renderer = std::make_shared<CSpriteRenderer>(_texture);

    _entities.push_back(_ballEntity);

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
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))    _player1Paddle->SetVelocity(1);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))      _player1Paddle->SetVelocity(-1);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))       _player2Paddle->SetVelocity(-1);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))       _player2Paddle->SetVelocity(1);
}

void Game::update(float dt)
{
    if (hasCollision(_ball->getCollisionRect(), _player1Paddle->getCollisionRect()) || hasCollision(_ball->getCollisionRect(), _player2Paddle->getCollisionRect()))
    {
        _ball->reverse();
    }

    for (int i = 0; i < _gameObjects.size(); i++)
    {
        _gameObjects[i]->update(dt);
    }

    // trying out a movement system
    for (auto ent : _entities)
    {
        ent->transform->position += ent->transform->velocity * dt;
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
        ent->renderer->sprite.setPosition(ent->transform->position.x, ent->transform->position.y);
        _window->draw(ent->renderer->sprite);
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

    if (!_texture.loadFromFile("notaBall.png"))
    {
        std::cout << "Couldn't find image notaBall.png";
        return false;
    }

    return true;
}
