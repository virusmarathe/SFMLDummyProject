#include "Scene_PhysicsTest.h"
#include "Framework/GameEngine.h"
#include "Physics/Physics.h"
#include "../Settings.h"
#include "Framework/Action.h"
#include "System/SRender.h"
#include "System/SMovement.h"
#include "System/SPhysics.h"
#include "Framework/Primitives.h"
#include "SFML/Network.hpp"
#include "System/STransformSync.h"

void Scene_PhysicsTest::init()
{
	_assets = _engine->getAssets();

    _player1Goal = _entities.addEntity("Goal");
    Rect blueRect(0, 0, 30, (float)_window->getSize().y);
    auto& p1GoalTransform = _player1Goal.addComponent<CTransform>();
    p1GoalTransform.position = blueRect.pos;
    auto& p1GoalrectCollider = _player1Goal.addComponent<CRectCollider>();
    p1GoalrectCollider.rect = blueRect;
    p1GoalrectCollider.isTrigger = true;
    Primitives::DrawRectShape(blueRect, sf::Color::Blue);

    _player2Goal = _entities.addEntity("Goal");
    Rect redRect((float)_window->getSize().x - 30, 0, 30, (float)_window->getSize().y);
    auto& p2GoalTransform = _player2Goal.addComponent<CTransform>();
    p2GoalTransform.position = redRect.pos;
    auto& p2GoalRectCollider = _player2Goal.addComponent<CRectCollider>();
    p2GoalRectCollider.rect = redRect;
    p2GoalRectCollider.isTrigger = true;
    Primitives::DrawRectShape(redRect, sf::Color::Red);

    spawnWall(Rect(0, 0, (float)_window->getSize().x, 50));
    spawnWall(Rect(0, (float)_window->getSize().y - 50, (float)_window->getSize().x, 50));
    spawnWall(Rect(-50, 0, 50, (float)_window->getSize().y));
    spawnWall(Rect((float)_window->getSize().x, 0, 50, (float)_window->getSize().y));

    if (NetworkManager::isServer)
    {
        spawnPlayerServer(Vector2(200.0f * (float)(rand() % 8), 200), NetworkManager::clientID);
    }

    _player1ScoreBoard = _entities.addEntity("Score");
    auto& p1ScoreTransform = _player1ScoreBoard.addComponent<CTransform>();
    p1ScoreTransform.position = Vector2(_window->getSize().x / 2.0f - 100, 60);
    auto& p1ScoreTextComp = _player1ScoreBoard.addComponent<CText>();
    p1ScoreTextComp.text = sf::Text(sf::String("Player 1: 0"), _assets->getFont("NormalUIFont"), 24);
    p1ScoreTextComp.textColor = sf::Color::Blue;
    _player1Score = 0;

    _player2ScoreBoard = _entities.addEntity("Score");
    auto& p2ScoreTransform = _player2ScoreBoard.addComponent<CTransform>();
    p2ScoreTransform.position = Vector2(_window->getSize().x / 2.0f - 100, 100);
    auto& p2ScoreTextComp = _player2ScoreBoard.addComponent<CText>();
    p2ScoreTextComp.text = sf::Text(sf::String("Player 2: 0"), _assets->getFont("NormalUIFont"), 24);
    p2ScoreTextComp.textColor = sf::Color::Red;
    _player2Score = 0;

    _ballTimer = 10;

    std::string idText = "Client ID: " + std::to_string(NetworkManager::clientID);
    Primitives::DrawText(idText.c_str(), Vector2(50,50), _assets->getFont("NormalUIFont"));

    auto camera = _entities.addEntity("Camera");
    auto& cameraTransform = camera.addComponent<CTransform>();
    cameraTransform.position = Vector2(_window->getSize().x / 2.0f, _window->getSize().y / 2.0f);
    cameraTransform.scale = Vector2(_window->getSize());

    // will eventually be moved to a config file
    _engine->registerAction(sf::Keyboard::P, "PHYSICS_TOGGLE");
    _engine->registerAction(sf::Keyboard::W, "UP");
    _engine->registerAction(sf::Keyboard::A, "LEFT");
    _engine->registerAction(sf::Keyboard::S, "DOWN");
    _engine->registerAction(sf::Keyboard::D, "RIGHT");
    _engine->registerAction(sf::Keyboard::Enter, "START");

    _engine->registerAction(sf::Keyboard::Num1, "DUNGEON_SCENE");

    //_engine->playBGMusic("Level2BG");

    _engine->registerSystem(std::make_shared<STransformSync>(_engine, &_entities, Priority::UPDATE));
    _engine->registerSystem(std::make_shared<SPhysics>(&_entities, Priority::PHYSICS));
    _engine->registerSystem(std::make_shared<SMovement>(&_entities, Priority::UPDATE));
    _engine->registerSystem(std::make_shared<SRender>(&_entities, Priority::RENDER, _window, camera));

    if (!NetworkManager::isServer)
    {
        sf::Packet packet;
        packet << NetworkManager::PacketType::LATE_CONNECT_REQUEST << NetworkManager::clientID;
        _engine->sendToServer(packet);
    }
}

void Scene_PhysicsTest::preUpdate(float dt)
{
    sInput();
}

void Scene_PhysicsTest::postUpdate(float dt)
{
    sHandleCollision(dt);
}

void Scene_PhysicsTest::sDoAction(const Action& action)
{
    if (action.name == "PHYSICS_TOGGLE" && action.type == Action::ActionType::START) GameEngine::DEBUG_MODE = !GameEngine::DEBUG_MODE;

    if (action.name == "UP") _playerEntity.getComponent<CInput>().up = action.type == Action::ActionType::START;
    if (action.name == "LEFT") _playerEntity.getComponent<CInput>().left = action.type == Action::ActionType::START;
    if (action.name == "DOWN") _playerEntity.getComponent<CInput>().down = action.type == Action::ActionType::START;
    if (action.name == "RIGHT") _playerEntity.getComponent<CInput>().right = action.type == Action::ActionType::START;

    if (action.name == "DUNGEON_SCENE" && action.type == Action::ActionType::START) _engine->changeScene("DungeonTest");

    if (action.name == "START" && action.type == Action::ActionType::START && NetworkManager::isServer) spawnBallServer();
}

enum PhysicsSceneNetworkEvents
{
    SPAWN_BALL,
    SPAWN_PLAYER
};

void Scene_PhysicsTest::handlePacket(sf::Packet& packet)
{
    int eventType;
    packet >> eventType;
    if (eventType == SPAWN_BALL)
    {
        spawnBallClient(packet);
    }
    else if (eventType == SPAWN_PLAYER)
    {
        spawnPlayerClient(packet);
    }
}

void Scene_PhysicsTest::onClientConnectedToServer(int clientID)
{
    spawnPlayerServer(Vector2(200.0f * (float)(rand() % 8), 500), clientID);
}

void Scene_PhysicsTest::spawnBallServer()
{
    int xVel = rand() % 2 == 1 ? 1 : -1;
    int yVel = rand() % 2 == 1 ? 1 : -1;
    Entity ball = _engine->getNetManager()->serverCreateEntity(&_entities, "Ball");
    auto& transform = ball.addComponent<CTransform>();
    transform.position = Vector2(_window->getSize().x / 2.0f, _window->getSize().y / 2.0f);
    transform.scale = Vector2(Settings::BALL_SIZE, Settings::BALL_SIZE);
    auto& sprite = ball.addComponent<CSprite>();
    sprite.sprite = sf::Sprite(_assets->getTexture("Ball"));
    auto& rectCollider = ball.addComponent<CRectCollider>();
    rectCollider.rect = Rect(ball.getComponent<CTransform>().position.x, ball.getComponent<CTransform>().position.y, Settings::BALL_SIZE, Settings::BALL_SIZE);
    auto& physics = ball.addComponent<CPhysicsBody>();
    physics.velocity = Vector2(Settings::BALL_START_SPEED * xVel, Settings::BALL_START_SPEED * yVel);
    physics.elastic = true;
    physics.collidesWith = 1;
    auto& netTransform = ball.addComponent<CNetworkTransform>();
    netTransform.position = transform.position;
    sf::Packet packet;
    packet << NetworkManager::PacketType::SCENE_EVENT << SPAWN_BALL << ball.getComponent<CNetID>().netID << transform.position;
    _engine->sendToAllClients(packet);
}

void Scene_PhysicsTest::spawnBallClient(sf::Packet& packet)
{
    unsigned int netID;
    Vector2 pos;
    packet >> netID >> pos;
    Entity ball = _entities.addEntity("Ball");
    auto& transform = ball.addComponent<CTransform>();
    transform.position = pos;
    transform.scale = Vector2(Settings::BALL_SIZE, Settings::BALL_SIZE);
    auto& sprite = ball.addComponent<CSprite>();
    sprite.sprite = sf::Sprite(_assets->getTexture("Ball"));
    auto& rectCollider = ball.addComponent<CRectCollider>();
    rectCollider.rect = Rect(ball.getComponent<CTransform>().position.x, ball.getComponent<CTransform>().position.y, Settings::BALL_SIZE, Settings::BALL_SIZE);
    auto& physics = ball.addComponent<CPhysicsBody>();
    physics.elastic = true;
    physics.collidesWith = 1;
    auto& netTrans = ball.addComponent<CNetworkTransform>();
    netTrans.position = pos;
    auto& netIDComp = ball.addComponent<CNetID>();
    netIDComp.netID = netID;
    _engine->getNetManager()->clientAddNetID(netID, ball);
}

void Scene_PhysicsTest::spawnWall(Rect rect)
{
    const float GRID_SIZE = 50;
    Entity wall = Primitives::TiledSprite(rect, GRID_SIZE, _assets->getTexture("Wall"), "Wall");
    auto& rectCollider = wall.addComponent<CRectCollider>();
    rectCollider.rect = rect;
}

void Scene_PhysicsTest::spawnPlayerServer(Vector2 pos, int clientID)
{
    auto player = _engine->getNetManager()->serverCreateEntity(&_entities, "Player");
    player.getComponent<CNetID>().ownerID = clientID;
    auto& transform = player.addComponent<CTransform>();
    transform.position = pos;
    auto& sprite = player.addComponent<CSprite>();
    sprite.sprite = sf::Sprite(_assets->getTexture("Paddle"));
    auto& rectCollider = player.addComponent<CRectCollider>();
    rectCollider.rect = Rect(sprite.sprite.getTextureRect());
    auto& physics = player.addComponent<CPhysicsBody>();
    physics.collidesWith = 1;
    auto& netTransform = player.addComponent<CNetworkTransform>();
    netTransform.position = pos;
    player.addComponent<CInput>();
    if (clientID == NetworkManager::clientID)
    {
        _playerEntity = player;
        _spawnedLocalPlayer = true;
    }
    sf::Packet packet;
    packet << NetworkManager::PacketType::SCENE_EVENT << SPAWN_PLAYER << player.getComponent<CNetID>().netID << pos << clientID;
    _engine->sendToAllClients(packet);
    _engine->addLateConnectPacket(packet);
}

void Scene_PhysicsTest::spawnPlayerClient(sf::Packet& packet)
{
    unsigned int netID;
    int clientID;
    Vector2 pos;
    packet >> netID >> pos >> clientID;
    if (clientID == NetworkManager::clientID && _spawnedLocalPlayer) { return; } // don't spawn yourself twice
    Entity player = _entities.addEntity("Player");
    auto& transform = player.addComponent<CTransform>();
    transform.position = pos;
    auto& sprite = player.addComponent<CSprite>();
    sprite.sprite = sf::Sprite(_assets->getTexture("Paddle"));
    auto& rectCollider = player.addComponent<CRectCollider>();
    rectCollider.rect = Rect(sprite.sprite.getTextureRect());
    auto& physics = player.addComponent<CPhysicsBody>();
    physics.collidesWith = 1;
    auto& netIDComp = player.addComponent<CNetID>();
    netIDComp.netID = netID;
    netIDComp.ownerID = clientID;
    auto& netTransform = player.addComponent<CNetworkTransform>();
    netTransform.position = pos;
    if (clientID == NetworkManager::clientID)
    {
        player.addComponent<CInput>();
        _playerEntity = player;
        _spawnedLocalPlayer = true;
    }
    _engine->getNetManager()->clientAddNetID(netID, player);
}

void Scene_PhysicsTest::sInput()
{
    for (auto ent : _entities.getEntities("Player"))
    {
        if (ent.hasComponent<CInput>() && ent.hasComponent<CPhysicsBody>())
        {
            auto& input = ent.getComponent<CInput>();
            Vector2 vel;

            if (input.up) vel.y -= Settings::PADDLE_SPEED;
            if (input.down) vel.y += Settings::PADDLE_SPEED;
            if (input.left) vel.x -= Settings::PADDLE_SPEED;
            if (input.right) vel.x += Settings::PADDLE_SPEED;

            ent.getComponent<CPhysicsBody>().velocity = vel;

            if (!NetworkManager::isServer)
            {
                sf::Packet packet;
                packet << NetworkManager::PacketType::INPUT_EVENT << ent.getComponent<CNetID>().netID << input;
                _engine->sendToServer(packet);
            }
        }
    }
}

void Scene_PhysicsTest::sHandleCollision(float dt)
{
    for (auto ent : _entities.getEntities("CollisionEvent"))
    {
        auto& cEvent = ent.getComponent<CCollisionEvent>();
        Entity ent1 = _entities[cEvent.ent1];
        Entity ent2 = _entities[cEvent.ent2];
        // handle ball collision with something
        if (ent1.tag() == "Ball")
        {
            if (ent2.tag() == "Goal")
            {
                if (NetworkManager::isServer)
                {
                    if (ent2.id == _player2Goal.id)
                    {
                        std::string scoreString = "Player 1: " + std::to_string(++_player1Score);
                        _player1ScoreBoard.getComponent<CText>().text.setString(scoreString);
                    }
                    else
                    {
                        std::string scoreString = "Player 2: " + std::to_string(++_player2Score);
                        _player2ScoreBoard.getComponent<CText>().text.setString(scoreString);
                    }
                    _engine->getNetManager()->serverDestroyEntity(ent1.getComponent<CNetID>().netID);
                    ent1.getComponent<CRectCollider>().enabled = false;
                }
            }
            else
            {
                //_engine->playSound("BallBounce");
            }
        }
    }
}