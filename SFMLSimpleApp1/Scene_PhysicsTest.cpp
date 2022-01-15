#include "Scene_PhysicsTest.h"
#include "Framework/GameEngine.h"
#include "Physics/Physics.h"
#include "Config.h"
#include "Framework/Action.h"
#include "System/SRender.h"
#include "System/SMovement.h"
#include "System/SPhysics.h"

void Scene_PhysicsTest::init()
{
	_assets = _engine->getAssets();

    spawnWall(Rect(0, 0, (float)_window->getSize().x, 50));
    spawnWall(Rect(0, (float)_window->getSize().y - 50, (float)_window->getSize().x, 50));
    spawnWall(Rect(0, 0, 50, (float)_window->getSize().y));
    spawnWall(Rect((float)_window->getSize().x - 50, 0, 50, (float)_window->getSize().y));

    _player1Entity = spawnPlayer(Vector2(200, 200), 0);
    _player2Entity = spawnPlayer(Vector2(800, 400), 1);

    auto player1Score = _entities.addEntity("Score");
    player1Score->addComponent<CTransform>(Vector2(_window->getSize().x / 2.0f, 20));
    player1Score->addComponent<CText>("Player 1: 0", _assets->getFont("NormalUIFont"), 24, sf::Color::Blue);

    auto player2Score = _entities.addEntity("Score");
    player2Score->addComponent<CTransform>(Vector2(_window->getSize().x / 2.0f, 60));
    player2Score->addComponent<CText>("Player 2: 0", _assets->getFont("NormalUIFont"), 24, sf::Color::Red);

    _ballTimer = 10;

    // will eventually be moved to a config file
    _engine->registerAction(sf::Keyboard::P, "PHYSICS_TOGGLE");
    _engine->registerAction(sf::Keyboard::W, "P1UP");
    _engine->registerAction(sf::Keyboard::A, "P1LEFT");
    _engine->registerAction(sf::Keyboard::S, "P1DOWN");
    _engine->registerAction(sf::Keyboard::D, "P1RIGHT");
    _engine->registerAction(sf::Keyboard::Up, "P2UP");
    _engine->registerAction(sf::Keyboard::Left, "P2LEFT");
    _engine->registerAction(sf::Keyboard::Right, "P2RIGHT");
    _engine->registerAction(sf::Keyboard::Down, "P2DOWN");

    _engine->registerAction(sf::Keyboard::Num1, "ANIMATION_SCENE");

    _engine->playBGMusic("Level2BG");

    _engine->registerSystem(std::make_shared<SPhysics>(&_entities, Priority::PHYSICS));
    _engine->registerSystem(std::make_shared<SMovement>(&_entities, Priority::UPDATE));
    _engine->registerSystem(std::make_shared<SRender>(&_entities, Priority::RENDER, _window));
}

void Scene_PhysicsTest::update(float dt)
{
    sHandleCollision(dt);

    _ballTimer += dt;
    if (_ballTimer >= 10)
    {
        spawnNewBall();
        _ballTimer = 0;
    }
}

void Scene_PhysicsTest::sDoAction(const Action& action)
{
    if (action.name == "PHYSICS_TOGGLE" && action.type == Action::ActionType::START)
    {
        GameEngine::DEBUG_MODE = !GameEngine::DEBUG_MODE;
        _debugToggle = !_debugToggle;
    }

    if (action.name == "P1UP") _player1Entity->getComponent<CInput>()->up = action.type == Action::ActionType::START;
    if (action.name == "P1LEFT") _player1Entity->getComponent<CInput>()->left = action.type == Action::ActionType::START;
    if (action.name == "P1DOWN") _player1Entity->getComponent<CInput>()->down = action.type == Action::ActionType::START;
    if (action.name == "P1RIGHT") _player1Entity->getComponent<CInput>()->right = action.type == Action::ActionType::START;
    if (action.name == "P2UP") _player2Entity->getComponent<CInput>()->up = action.type == Action::ActionType::START;
    if (action.name == "P2LEFT") _player2Entity->getComponent<CInput>()->left = action.type == Action::ActionType::START;
    if (action.name == "P2DOWN") _player2Entity->getComponent<CInput>()->down = action.type == Action::ActionType::START;
    if (action.name == "P2RIGHT") _player2Entity->getComponent<CInput>()->right = action.type == Action::ActionType::START;

    if (action.name == "ANIMATION_SCENE" && action.type == Action::ActionType::START) _engine->changeScene("AnimationTest");

    sInput();
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

std::shared_ptr<Entity> Scene_PhysicsTest::spawnPlayer(Vector2 pos, int playerNum)
{
    auto player = _entities.addEntity("Player");
    player->addComponent<CTransform>(pos);
    std::shared_ptr<CSprite> sprite = player->addComponent<CSprite>(_assets->getTexture("Paddle"));
    player->addComponent<CRectCollider>(Rect(sprite->sprite.getTextureRect()));
    player->addComponent<CPhysicsBody>();
    player->addComponent<CInput>();

    return player;
}

void Scene_PhysicsTest::sInput()
{
    for (auto ent : _entities.getEntities("Player"))
    {
        if (ent->hasComponent<CInput>() && ent->hasComponent<CPhysicsBody>())
        {
            std::shared_ptr<CInput> input = ent->getComponent<CInput>();
            Vector2 vel;

            if (input->up)     vel.y -= PADDLE_SPEED;
            if (input->down)   vel.y += PADDLE_SPEED;
            if (input->left)   vel.x -= PADDLE_SPEED;
            if (input->right)  vel.x += PADDLE_SPEED;

            ent->getComponent<CPhysicsBody>()->velocity = vel;
        }
    }
}

void Scene_PhysicsTest::sHandleCollision(float dt)
{
    for (auto ent : _entities.getEntities("CollisionEvent"))
    {
        std::shared_ptr<CCollisionEvent> cEvent = ent->getComponent<CCollisionEvent>();
        // handle ball collision with something
        if (cEvent->ent1->tag() == "Ball")
        {
            _engine->playSound("BallBounce");
        }
    }
}