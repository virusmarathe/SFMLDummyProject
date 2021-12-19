#include "Game.h"
#include <iostream>
#include "Config.h"
#include "Math/Vector2.h"
#include "Entity/Entity.h"
#include "Math/Rect.h"
#include "Physics/Physics.h"

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
    _player1Entity->physics = std::make_shared<CPhysicsBody>();

    _player2Entity = _entities.addEntity("Player");
    _player2Entity->transform = std::make_shared<CTransform>(Vector2((float)_window->getSize().x - PADDLE_WIDTH, 200));
    _player2Entity->sprite = std::make_shared<CSprite>(_paddleTexture);
    _player2Entity->collider = std::make_shared<CRectCollider>(Rect(_player2Entity->sprite->sprite.getTextureRect()));
    _player2Entity->physics = std::make_shared<CPhysicsBody>();

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

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))       _player1Entity->physics->velocity.y = -PADDLE_SPEED;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))       _player1Entity->physics->velocity.y = PADDLE_SPEED;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))       _player1Entity->physics->velocity.x = -PADDLE_SPEED;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))       _player1Entity->physics->velocity.x = PADDLE_SPEED;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))    _player2Entity->physics->velocity.y = PADDLE_SPEED;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))      _player2Entity->physics->velocity.y = -PADDLE_SPEED;
}

void Game::update(float dt)
{
    _entities.update();

    sPhysics(dt);

    // trying out a movement system
    for (auto ent : _entities.getEntities())
    {
        if (ent->transform && ent->physics)
        {
            ent->transform->position += ent->physics->velocity * dt;
            if (ent->collider)
            {
                ent->collider->rect.pos = ent->transform->position;
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

Vector2 debugStart(100, 100);

void Game::render()
{
    _window->clear();

    Vector2 mousePos = sf::Mouse::getPosition(*_window);

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
            if (Physics::checkCollision(mousePos, rect))
            {
                line[0].color = line[1].color = line[2].color = line[3].color = line[4].color = sf::Color::Red;
            }

            _window->draw(line, 5, sf::LineStrip);
        }
    }

    _window->display();
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
        Vector2((BALL_SIZE * 2) / _ballTexture.getSize().x, (BALL_SIZE * 2) / _ballTexture.getSize().y));
    ball->sprite = std::make_shared<CSprite>(_ballTexture);
    ball->sprite->sprite.setScale(ball->transform->scale.x, ball->transform->scale.y);
    ball->collider = std::make_shared<CRectCollider>(Rect(ball->transform->position.x, ball->transform->position.y, BALL_SIZE * 2, BALL_SIZE * 2));
    ball->physics = std::make_shared<CPhysicsBody>(Vector2(BALL_START_SPEED * xVel, BALL_START_SPEED * yVel), true);
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

void Game::sPhysics(float dt)
{
    for (auto ent : _entities.getEntities())
    {
        if (ent->collider && ent->physics)
        {
            for (auto ent2 : _entities.getEntities())
            {
                if (ent == ent2) continue;
                if (ent2->collider)
                {
                    Vector2 halfOffset = (ent->collider->rect.size / 2.0f);
                    Vector2 rayPos = ent->collider->rect.pos + halfOffset;
                    Rect expandedRect(ent2->collider->rect.pos - halfOffset, ent2->collider->rect.size + ent->collider->rect.size);
                    Vector2 contactPoint;
                    Vector2 normal;
                    float hitTime;

                    if (Physics::checkCollision(rayPos, ent->physics->velocity * dt, expandedRect, contactPoint, normal, hitTime))
                    {
                        if (ent->physics->elastic)
                        {
                            // calculate reflection vector
                            ent->physics->velocity = (normal * (-1 * ent->physics->velocity.dot(normal)) * 2) + ent->physics->velocity;                     
                        }
                        else
                        {
                            ent->physics->velocity -= (normal * (ent->physics->velocity * dt * (1 - hitTime)).dot(normal)) / dt;
                        }
                    }
                }
            }
        }
    }
}
