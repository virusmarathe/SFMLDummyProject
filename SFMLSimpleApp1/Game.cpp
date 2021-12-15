#include "Game.h"
#include <iostream>
#include "Config.h"
#include "Engine/Math/Vector2.h"
#include "Engine/Entity/Entity.h"

void Game::init(sf::RenderWindow * window)
{
    loadResources();

    _window = window;

    spawnNewBall();

    std::shared_ptr<Entity> topWall = _entities.addEntity("Wall");
    topWall->transform = std::make_shared<CTransform>(Vector2(0, 0));
    topWall->collider = std::make_shared<CRectCollider>(sf::FloatRect(0,0,(float)_window->getSize().x, 50));

    std::shared_ptr<Entity> bottomWall = _entities.addEntity("Wall");
    bottomWall->transform = std::make_shared<CTransform>(Vector2(0, (float)_window->getSize().y - 50));
    bottomWall->collider = std::make_shared<CRectCollider>(sf::FloatRect(0, (float)_window->getSize().y - 50, (float)_window->getSize().x, 50));

    _player1Entity = _entities.addEntity("Player");
    _player1Entity->transform = std::make_shared<CTransform>(Vector2(500, 200));
    _player1Entity->sprite = std::make_shared<CSprite>(_paddleTexture);
    _player1Entity->collider = std::make_shared<CRectCollider>(sf::FloatRect(_player1Entity->sprite->sprite.getTextureRect()));

    _player2Entity = _entities.addEntity("Player");
    _player2Entity->transform = std::make_shared<CTransform>(Vector2((float)_window->getSize().x - PADDLE_WIDTH, 200));
    _player2Entity->sprite = std::make_shared<CSprite>(_paddleTexture);
    _player2Entity->collider = std::make_shared<CRectCollider>(sf::FloatRect(_player2Entity->sprite->sprite.getTextureRect()));
    
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
                ent->collider->collider.left = ent->transform->position.x;
                ent->collider->collider.top = ent->transform->position.y;
            }
        }
    }

    for (auto ent : _entities.getEntities())
    {
        if (ent->collider)
        {
            for (auto ball : _entities.getEntities("Ball"))
            {
                if (ball && ent != ball && hasCollision(ball->collider->collider, ent->collider->collider))
                {
                    ball->transform->velocity *= -1;
                }
            }
            for (auto player : _entities.getEntities("Player"))
            {
                if (ent->tag() == "Wall" && hasCollision(player->collider->collider, ent->collider->collider))
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
    ball->collider = std::make_shared<CRectCollider>(sf::FloatRect(ball->transform->position.x, ball->transform->position.y, BALL_SIZE * 2, BALL_SIZE * 2));
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
