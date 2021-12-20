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

    spawnWall(Rect(0, 0, (float)_window->getSize().x, 50));
    spawnWall(Rect(0, (float)_window->getSize().y - 50, (float)_window->getSize().x, 50));
    spawnWall(Rect(0, 0, 50, (float)_window->getSize().y));
    spawnWall(Rect((float)_window->getSize().x - 50, 0, 50, (float)_window->getSize().y));

    spawnPlayer(Vector2(200, 200), 0);
    spawnPlayer(Vector2(800, 400), 1);

    _player1Score = _entities.addEntity("Score");
    _player1Score->transform = std::make_shared<CTransform>(Vector2(_window->getSize().x / 2.0f, 20));
    _player1Score->text = std::make_shared<CText>("Player 1: 0",_font, 24, sf::Color::Blue);

    _player2Score = _entities.addEntity("Score");
    _player2Score->transform = std::make_shared<CTransform>(Vector2(_window->getSize().x / 2.0f, 60));
    _player2Score->text = std::make_shared<CText>("Player 2: 0", _font, 24, sf::Color::Red);

    _ballTimer = 10;
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

    sInput();
}

void Game::update(float dt)
{
    _entities.update();

    sPhysics(dt);
    sMovement(dt);

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

    sRender();
    if (_debugToggle)
    {
        sDebugDraw();
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
    float ballSize = BALL_SIZE * 2 * ((float)(rand() % 3 + 1) / ((float)(rand() % 4 + 1)));
    ball->transform = std::make_shared<CTransform>(Vector2(400,300),
        Vector2((ballSize) / _ballTexture.getSize().x, (ballSize) / _ballTexture.getSize().y));
    ball->sprite = std::make_shared<CSprite>(_ballTexture);
    ball->sprite->sprite.setScale(ball->transform->scale.x, ball->transform->scale.y);
    ball->collider = std::make_shared<CRectCollider>(Rect(ball->transform->position.x, ball->transform->position.y, ballSize, ballSize));
    ball->physics = std::make_shared<CPhysicsBody>(Vector2(BALL_START_SPEED * xVel, BALL_START_SPEED * yVel), true);
}

void Game::spawnWall(Rect rect)
{
    std::shared_ptr<Entity> wall = _entities.addEntity("Wall");
    wall->transform = std::make_shared<CTransform>(rect.pos);
    wall->collider = std::make_shared<CRectCollider>(rect);
}

void Game::spawnPlayer(Vector2 pos, int playerNum)
{
    auto player = _entities.addEntity("Player");
    player->transform = std::make_shared<CTransform>(pos);
    player->sprite = std::make_shared<CSprite>(_paddleTexture);
    player->collider = std::make_shared<CRectCollider>(Rect(player->sprite->sprite.getTextureRect()));
    player->physics = std::make_shared<CPhysicsBody>();
    player->controller = std::make_shared<CPlayerController>(PLAYER_CONTROLS[playerNum][0], PLAYER_CONTROLS[playerNum][1], PLAYER_CONTROLS[playerNum][2], PLAYER_CONTROLS[playerNum][3]);
    player->input = std::make_shared<CInput>();
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

void Game::sInput()
{
    for (auto ent : _entities.getEntities("Player"))
    {
        if (ent->input && ent->controller)
        {
            ent->input->up = sf::Keyboard::isKeyPressed(ent->controller->upKey);
            ent->input->down = sf::Keyboard::isKeyPressed(ent->controller->downKey);
            ent->input->left = sf::Keyboard::isKeyPressed(ent->controller->leftKey);
            ent->input->right = sf::Keyboard::isKeyPressed(ent->controller->rightKey);

            Vector2 vel;
            if (ent->input->up)     vel.y -= PADDLE_SPEED;
            if (ent->input->down)   vel.y += PADDLE_SPEED;
            if (ent->input->left)   vel.x -= PADDLE_SPEED;
            if (ent->input->right)  vel.x += PADDLE_SPEED;

            ent->physics->velocity = vel;
        }
    }
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

void Game::sMovement(float dt)
{
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
}

void Game::sRender()
{
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
    }
}

void Game::sDebugDraw()
{
    Vector2 mousePos = sf::Mouse::getPosition(*_window);
    for (auto ent : _entities.getEntities())
    {
        if (ent->collider)
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
}
