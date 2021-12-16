#include "Game.h"
#include <iostream>
#include "Config.h"
#include "Math/Vector2.h"
#include "Entity/Entity.h"
#include "Math/Rect.h"

void Game::init(sf::RenderWindow * window)
{
    loadResources();

    _window = window;

    spawnNewBall();

    std::shared_ptr<Entity> topWall = _entities.addEntity("Wall");
    topWall->transform = std::make_shared<CTransform>(Vector2(0, 0));
    topWall->collider = std::make_shared<CRectCollider>(Rect(0,0,(float)_window->getSize().x, 50));

    std::shared_ptr<Entity> bottomWall = _entities.addEntity("Wall");
    bottomWall->transform = std::make_shared<CTransform>(Vector2(0, (float)_window->getSize().y - 50));
    bottomWall->collider = std::make_shared<CRectCollider>(Rect(0, (float)_window->getSize().y - 50, (float)_window->getSize().x, 50));

    _player1Entity = _entities.addEntity("Player");
    _player1Entity->transform = std::make_shared<CTransform>(Vector2(500, 200));
    _player1Entity->sprite = std::make_shared<CSprite>(_paddleTexture);
    _player1Entity->collider = std::make_shared<CRectCollider>(Rect(_player1Entity->sprite->sprite.getTextureRect()));

    _player2Entity = _entities.addEntity("Player");
    _player2Entity->transform = std::make_shared<CTransform>(Vector2((float)_window->getSize().x - PADDLE_WIDTH, 200));
    _player2Entity->sprite = std::make_shared<CSprite>(_paddleTexture);
    _player2Entity->collider = std::make_shared<CRectCollider>(Rect(_player2Entity->sprite->sprite.getTextureRect()));
    
    _player1Score = _entities.addEntity("Score");
    _player1Score->transform = std::make_shared<CTransform>(Vector2(_window->getSize().x / 2.0f, 20));
    _player1Score->text = std::make_shared<CText>("Player 1: 0",_font, 24, sf::Color::Blue);

    _player2Score = _entities.addEntity("Score");
    _player2Score->transform = std::make_shared<CTransform>(Vector2(_window->getSize().x / 2.0f, 60));
    _player2Score->text = std::make_shared<CText>("Player 1: 0", _font, 24, sf::Color::Red);
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
    _entities.update();

    // trying out a movement system
    for (auto ent : _entities.getEntities())
    {
        if (ent->transform)
        {
            ent->transform->position += ent->transform->velocity * dt;
            if (ent->collider)
            {
                ent->collider->rect.pos = ent->transform->position;
            }
        }
    }

    for (auto ent : _entities.getEntities())
    {
        if (ent->collider)
        {
            for (auto ball : _entities.getEntities("Ball"))
            {
                if (ball && ent != ball && hasCollision(ball->collider->rect, ent->collider->rect))
                {
                    ball->transform->velocity *= -1;
                }
            }
            for (auto player : _entities.getEntities("Player"))
            {
                if (ent->tag() == "Wall" && hasCollision(player->collider->rect, ent->collider->rect))
                {
                    player->transform->velocity.y = 0;
                }
            }
        }
    }

    _ballTimer += dt;
    if (_ballTimer >= 10)
    {
        spawnNewBall();
        _ballTimer = 0;
    }
}

void Game::render()
{
    _window->clear();

    // trying out a rendering system
    for (auto ent : _entities.getEntities())
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
            Rect rect = ent->collider->rect;
            sf::Vertex line[] =
            {
                sf::Vertex(sf::Vector2f(rect.pos.x, rect.pos.y)),
                sf::Vertex(sf::Vector2f(rect.pos.x + rect.size.x, rect.pos.y)),
                sf::Vertex(sf::Vector2f(rect.pos.x + rect.size.x, rect.pos.y + rect.size.y)),
                sf::Vertex(sf::Vector2f(rect.pos.x, rect.pos.y + rect.size.y)),
                sf::Vertex(sf::Vector2f(rect.pos.x, rect.pos.y))
            };
            line[0].color = line[1].color = line[2].color = line[3].color = line[4].color = sf::Color::Green;
            _window->draw(line, 5, sf::LineStrip);
        }
    }

    _window->display();
}

bool Game::hasCollision(const Rect& shape1, const Rect& shape2)
{
    return (shape1.pos.x < shape2.pos.x + shape2.size.x &&
        shape1.pos.x + shape1.size.x > shape2.pos.x &&
        shape1.pos.y < shape2.pos.y + shape2.size.y &&
        shape1.pos.y + shape1.size.y > shape2.pos.y);
}

void Game::notifyBallScored(int playerNum)
{
    // TODO
}

void Game::spawnNewBall()
{
    int xVel = rand() % 2 == 1 ? 1 : -1;
    int yVel = rand() % 2 == 1 ? 1 : -1;
    std::shared_ptr<Entity> ball = _entities.addEntity("Ball");
    ball->transform = std::make_shared<CTransform>(Vector2(50.0f + rand() % (_window->getSize().x - 50), 100.0f + rand() % (_window->getSize().y - 100)),
        Vector2((BALL_SIZE * 2) / _ballTexture.getSize().x, (BALL_SIZE * 2) / _ballTexture.getSize().y),
        Vector2(BALL_START_SPEED * xVel, BALL_START_SPEED * yVel));
    ball->sprite = std::make_shared<CSprite>(_ballTexture);
    ball->sprite->sprite.setScale(ball->transform->scale.x, ball->transform->scale.y);
    ball->collider = std::make_shared<CRectCollider>(Rect(ball->transform->position.x, ball->transform->position.y, BALL_SIZE * 2, BALL_SIZE * 2));
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
