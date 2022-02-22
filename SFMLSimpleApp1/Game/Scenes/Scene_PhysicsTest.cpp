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
    _player1Goal->addComponent<CTransform>(blueRect.pos);
    _player1Goal->addComponent<CRectCollider>(blueRect, true);
    Primitives::DrawRectShape(blueRect, sf::Color::Blue);

    _player2Goal = _entities.addEntity("Goal");
    Rect redRect((float)_window->getSize().x - 30, 0, 30, (float)_window->getSize().y);
    _player2Goal->addComponent<CTransform>(redRect.pos);
    _player2Goal->addComponent<CRectCollider>(redRect, true);
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
    _player1ScoreBoard->addComponent<CTransform>(Vector2(_window->getSize().x / 2.0f - 100, 60));
    _player1ScoreBoard->addComponent<CText>("Player 1: 0", _assets->getFont("NormalUIFont"), 24, sf::Color::Blue);
    _player1Score = 0;

    _player2ScoreBoard = _entities.addEntity("Score");
    _player2ScoreBoard->addComponent<CTransform>(Vector2(_window->getSize().x / 2.0f - 100, 100));
    _player2ScoreBoard->addComponent<CText>("Player 2: 0", _assets->getFont("NormalUIFont"), 24, sf::Color::Red);
    _player2Score = 0;

    _ballTimer = 10;

    std::string idText = "Client ID: " + std::to_string(NetworkManager::clientID);
    Primitives::DrawText(idText.c_str(), Vector2(50,50), _assets->getFont("NormalUIFont"));

    auto camera = _entities.addEntity("Camera");
    camera->addComponent<CTransform>(Vector2(_window->getSize().x / 2.0f, _window->getSize().y / 2.0f), Vector2(_window->getSize()));

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
    _engine->registerSystem(std::make_shared<SRender>(&_entities, Priority::RENDER, _window));

    if (!NetworkManager::isServer)
    {
        sf::Packet packet;
        packet << NetworkManager::PacketType::LATE_CONNECT_REQUEST << NetworkManager::clientID;
        _engine->sendToServer(packet);
    }
}

void Scene_PhysicsTest::update(float dt)
{
    sHandleCollision(dt);
    sInput();
}

void Scene_PhysicsTest::sDoAction(const Action& action)
{
    if (action.name == "PHYSICS_TOGGLE" && action.type == Action::ActionType::START) GameEngine::DEBUG_MODE = !GameEngine::DEBUG_MODE;

    if (_playerEntity)
    {
        if (action.name == "UP") _playerEntity->getComponent<CInput>()->up = action.type == Action::ActionType::START;
        if (action.name == "LEFT") _playerEntity->getComponent<CInput>()->left = action.type == Action::ActionType::START;
        if (action.name == "DOWN") _playerEntity->getComponent<CInput>()->down = action.type == Action::ActionType::START;
        if (action.name == "RIGHT") _playerEntity->getComponent<CInput>()->right = action.type == Action::ActionType::START;
    }

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
    std::shared_ptr<Entity> ball = _engine->getNetManager()->serverCreateEntity(&_entities, "Ball");
    std::shared_ptr<CTransform> transform = ball->addComponent<CTransform>(Vector2(_window->getSize().x / 2.0f, _window->getSize().y / 2.0f), Vector2(Settings::BALL_SIZE, Settings::BALL_SIZE));
    ball->addComponent<CSprite>(_assets->getTexture("Ball"));
    ball->addComponent<CRectCollider>(Rect(ball->getComponent<CTransform>()->position.x, ball->getComponent<CTransform>()->position.y, Settings::BALL_SIZE, Settings::BALL_SIZE));
    ball->addComponent<CPhysicsBody>(Vector2(Settings::BALL_START_SPEED * xVel, Settings::BALL_START_SPEED * yVel), true);
    ball->addComponent<CNetworkTransform>(transform->position);
    sf::Packet packet;
    packet << NetworkManager::PacketType::SCENE_EVENT << SPAWN_BALL << ball->getComponent<CNetID>()->netID << transform->position;
    _engine->sendToAllClients(packet);
}

void Scene_PhysicsTest::spawnBallClient(sf::Packet& packet)
{
    unsigned int netID;
    Vector2 pos;
    packet >> netID >> pos;
    std::shared_ptr<Entity> ball = _entities.addEntity("Ball");
    ball->addComponent<CTransform>(pos, Vector2(Settings::BALL_SIZE, Settings::BALL_SIZE));
    ball->addComponent<CSprite>(_assets->getTexture("Ball"));
    ball->addComponent<CRectCollider>(Rect(ball->getComponent<CTransform>()->position.x, ball->getComponent<CTransform>()->position.y, Settings::BALL_SIZE, Settings::BALL_SIZE));
    ball->addComponent<CPhysicsBody>(Vector2(), true);
    ball->addComponent<CNetworkTransform>(pos);
    ball->addComponent<CNetID>(netID);
    _engine->getNetManager()->clientAddNetID(netID, ball);
}

void Scene_PhysicsTest::spawnWall(Rect rect)
{
    const float GRID_SIZE = 50;
    std::shared_ptr<Entity> wall = Primitives::TiledSprite(rect, GRID_SIZE, _assets->getTexture("Wall"), "Wall");
    wall->addComponent<CRectCollider>(rect);
}

void Scene_PhysicsTest::spawnPlayerServer(Vector2 pos, int clientID)
{
    auto player = _engine->getNetManager()->serverCreateEntity(&_entities, "Player");
    player->getComponent<CNetID>()->ownerID = clientID;
    player->addComponent<CTransform>(pos);
    std::shared_ptr<CSprite> sprite = player->addComponent<CSprite>(_assets->getTexture("Paddle"));
    player->addComponent<CRectCollider>(Rect(sprite->sprite.getTextureRect()));
    player->addComponent<CPhysicsBody>();
    player->addComponent<CNetworkTransform>(pos);
    player->addComponent<CInput>();
    if (clientID == NetworkManager::clientID)
    {
        _playerEntity = player;
    }
    sf::Packet packet;
    packet << NetworkManager::PacketType::SCENE_EVENT << SPAWN_PLAYER << player->getComponent<CNetID>()->netID << pos << clientID;
    _engine->sendToAllClients(packet);
    _engine->addLateConnectPacket(packet);
}

void Scene_PhysicsTest::spawnPlayerClient(sf::Packet& packet)
{
    unsigned int netID;
    int clientID;
    Vector2 pos;
    packet >> netID >> pos >> clientID;
    if (clientID == NetworkManager::clientID && _playerEntity != nullptr) { return; } // don't spawn yourself twice
    std::shared_ptr<Entity> player = _entities.addEntity("Player");
    player->addComponent<CTransform>(pos);
    std::shared_ptr<CSprite> sprite = player->addComponent<CSprite>(_assets->getTexture("Paddle"));
    player->addComponent<CRectCollider>(Rect(sprite->sprite.getTextureRect()));
    player->addComponent<CPhysicsBody>();
    player->addComponent<CNetID>(netID, clientID);
    player->addComponent<CNetworkTransform>(pos);
    if (clientID == NetworkManager::clientID)
    {
        player->addComponent<CInput>();
        _playerEntity = player;
    }
    _engine->getNetManager()->clientAddNetID(netID, player);
}

void Scene_PhysicsTest::sInput()
{
    for (auto ent : _entities.getEntities("Player"))
    {
        if (ent->hasComponent<CInput>() && ent->hasComponent<CPhysicsBody>())
        {
            std::shared_ptr<CInput> input = ent->getComponent<CInput>();
            Vector2 vel;

            if (input->up)     
                vel.y -= Settings::PADDLE_SPEED;
            if (input->down)   
                vel.y += Settings::PADDLE_SPEED;
            if (input->left)   
                vel.x -= Settings::PADDLE_SPEED;
            if (input->right)  
                vel.x += Settings::PADDLE_SPEED;

            ent->getComponent<CPhysicsBody>()->velocity = vel;

            if (!NetworkManager::isServer)
            {
                sf::Packet packet;
                packet << NetworkManager::PacketType::INPUT_EVENT << ent->getComponent<CNetID>()->netID << input;
                _engine->sendToServer(packet);
            }
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
            if (cEvent->ent2->tag() == "Goal")
            {
                if (NetworkManager::isServer)
                {
                    if (cEvent->ent2 == _player2Goal)
                    {
                        std::string scoreString = "Player 1: " + std::to_string(++_player1Score);
                        _player1ScoreBoard->getComponent<CText>()->text.setString(scoreString);
                    }
                    else
                    {
                        std::string scoreString = "Player 2: " + std::to_string(++_player2Score);
                        _player2ScoreBoard->getComponent<CText>()->text.setString(scoreString);
                    }
                    _engine->getNetManager()->serverDestroyEntity(cEvent->ent1->getComponent<CNetID>()->netID);
                    cEvent->ent1->getComponent<CRectCollider>()->enabled = false;
                }
            }
            else
            {
                //_engine->playSound("BallBounce");
            }
        }
    }
}