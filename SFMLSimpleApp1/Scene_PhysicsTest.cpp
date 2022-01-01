#include "Scene_PhysicsTest.h"
#include "Framework/GameEngine.h"
#include "Physics/Physics.h"
#include "Config.h"

void Scene_PhysicsTest::init()
{
	_assets = _engine->getAssets();

    spawnWall(Rect(0, 0, (float)_window->getSize().x, 50));
    spawnWall(Rect(0, (float)_window->getSize().y - 50, (float)_window->getSize().x, 50));
    spawnWall(Rect(0, 0, 50, (float)_window->getSize().y));
    spawnWall(Rect((float)_window->getSize().x - 50, 0, 50, (float)_window->getSize().y));

    spawnPlayer(Vector2(200, 200), 0);
    spawnPlayer(Vector2(800, 400), 1);

    auto player1Score = _entities.addEntity("Score");
    player1Score->addComponent<CTransform>(Vector2(_window->getSize().x / 2.0f, 20));
    player1Score->addComponent<CText>("Player 1: 0", _assets->getFont("NormalUIFont"), 24, sf::Color::Blue);

    auto player2Score = _entities.addEntity("Score");
    player2Score->addComponent<CTransform>(Vector2(_window->getSize().x / 2.0f, 60));
    player2Score->addComponent<CText>("Player 2: 0", _assets->getFont("NormalUIFont"), 24, sf::Color::Red);

    _ballTimer = 10;
}

void Scene_PhysicsTest::update(float dt)
{
    sInput();

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

void Scene_PhysicsTest::sDoAction()
{
}

void Scene_PhysicsTest::sRender()
{
    for (auto ent : _entities.getEntities())
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
    }

    sDebugDraw();
}

void Scene_PhysicsTest::spawnNewBall()
{
    int xVel = rand() % 2 == 1 ? 1 : -1;
    int yVel = rand() % 2 == 1 ? 1 : -1;
    std::shared_ptr<Entity> ball = _entities.addEntity("Ball");
    ball->addComponent<CTransform>(Vector2(400, 300), Vector2(BALL_SIZE, BALL_SIZE));
    ball->addComponent<CSprite>(_assets->getTexture("Ball"));
    ball->addComponent<CRectCollider>(Rect(ball->getComponent<CTransform>()->position.x, ball->getComponent<CTransform>()->position.y, BALL_SIZE, BALL_SIZE));
    ball->addComponent<CPhysicsBody>(Vector2(BALL_START_SPEED * xVel, BALL_START_SPEED * yVel), true);
}

void Scene_PhysicsTest::spawnWall(Rect rect)
{
    std::shared_ptr<Entity> wall = _entities.addEntity("Wall");
    wall->addComponent<CTransform>(rect.pos);
    wall->addComponent<CRectCollider>(rect);
}

void Scene_PhysicsTest::spawnPlayer(Vector2 pos, int playerNum)
{
    auto player = _entities.addEntity("Player");
    player->addComponent<CTransform>(pos);
    std::shared_ptr<CSprite> sprite = player->addComponent<CSprite>(_assets->getTexture("Paddle"));
    player->addComponent<CRectCollider>(Rect(sprite->sprite.getTextureRect()));
    player->addComponent<CPhysicsBody>();
    player->addComponent<CPlayerController>(PLAYER_CONTROLS[playerNum][0], PLAYER_CONTROLS[playerNum][1], PLAYER_CONTROLS[playerNum][2], PLAYER_CONTROLS[playerNum][3]);
    player->addComponent<CInput>();
}

void Scene_PhysicsTest::sInput()
{
    for (auto ent : _entities.getEntities("Player"))
    {
        if (ent->hasComponent<CInput>() && ent->hasComponent<CPlayerController>() && ent->hasComponent<CPhysicsBody>())
        {
            std::shared_ptr<CInput> input = ent->getComponent<CInput>();
            std::shared_ptr<CPlayerController> controller = ent->getComponent<CPlayerController>();

            input->up = sf::Keyboard::isKeyPressed(controller->upKey);
            input->down = sf::Keyboard::isKeyPressed(controller->downKey);
            input->left = sf::Keyboard::isKeyPressed(controller->leftKey);
            input->right = sf::Keyboard::isKeyPressed(controller->rightKey);

            Vector2 vel;
            if (input->up)     vel.y -= PADDLE_SPEED;
            if (input->down)   vel.y += PADDLE_SPEED;
            if (input->left)   vel.x -= PADDLE_SPEED;
            if (input->right)  vel.x += PADDLE_SPEED;

            ent->getComponent<CPhysicsBody>()->velocity = vel;
        }
    }
}

void Scene_PhysicsTest::sPhysics(float dt)
{
    for (auto ent : _entities.getEntities())
    {
        if (ent->hasComponent<CRectCollider>() && ent->hasComponent<CPhysicsBody>())
        {
            for (auto ent2 : _entities.getEntities())
            {
                if (ent == ent2) continue;
                if (ent2->hasComponent<CRectCollider>())
                {
                    std::shared_ptr<CRectCollider> ent1Collider = ent->getComponent<CRectCollider>();
                    std::shared_ptr<CRectCollider> ent2Collider = ent2->getComponent<CRectCollider>();
                    std::shared_ptr<CPhysicsBody> physics = ent->getComponent<CPhysicsBody>();

                    Vector2 halfOffset = (ent1Collider->rect.size / 2.0f);
                    Vector2 rayPos = ent1Collider->rect.pos + halfOffset;
                    Rect expandedRect(ent2Collider->rect.pos - halfOffset, ent2Collider->rect.size + ent1Collider->rect.size);
                    Vector2 contactPoint;
                    Vector2 normal;
                    float hitTime;

                    if (Physics::checkCollision(rayPos, physics->velocity * dt, expandedRect, contactPoint, normal, hitTime))
                    {
                        if (physics->elastic)
                        {
                            // calculate reflection vector
                            physics->velocity = (normal * (-1 * physics->velocity.dot(normal)) * 2) + physics->velocity;
                        }
                        else
                        {
                            physics->velocity -= (normal * (physics->velocity * dt * (1 - hitTime)).dot(normal)) / dt;
                        }
                    }
                }
            }
        }
    }
}

void Scene_PhysicsTest::sMovement(float dt)
{
    for (auto ent : _entities.getEntities())
    {
        if (ent->hasComponent<CTransform>() && ent->hasComponent<CPhysicsBody>())
        {
            ent->getComponent<CTransform>()->position += ent->getComponent<CPhysicsBody>()->velocity * dt;
            if (ent->hasComponent<CRectCollider>())
            {
                ent->getComponent<CRectCollider>()->rect.pos = ent->getComponent<CTransform>()->position;
            }
        }
    }
}

void Scene_PhysicsTest::sDebugDraw()
{
    Vector2 mousePos = sf::Mouse::getPosition(*_window);
    for (auto ent : _entities.getEntities())
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