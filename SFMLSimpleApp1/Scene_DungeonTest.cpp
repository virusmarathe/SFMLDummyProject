#include "Scene_DungeonTest.h"
#include "Framework/GameEngine.h"
#include "Resources/Assets.h"
#include "Framework/Action.h"
#include "Config.h"
#include "System/SRender.h"
#include "System/SMovement.h"
#include "System/SAnimation.h"
#include "System/SPhysics.h"
#include <iostream>

void Scene_DungeonTest::init()
{
    _assets = _engine->getAssets();

    Rect groundRect(-5000, -5000, 10000, 10000);
    std::shared_ptr<Entity> ground = _entities.addEntity("Ground");
    ground->addComponent<CTransform>(groundRect.pos);
    std::shared_ptr<CSprite> groundSpriteComp = ground->addComponent<CSprite>(_assets->getTexture("Ground"));
    groundSpriteComp->sprite.setTextureRect(sf::IntRect(0, 0, 15000, 15000));

    _camera = _entities.addEntity("Camera");
    _camera->addComponent<CTransform>(Vector2(), Vector2(_window->getSize()));
    _camera->addComponent<CPhysicsBody>();

    _engine->registerAction(sf::Keyboard::P, "PHYSICS_TOGGLE");
    _engine->registerAction(sf::Keyboard::W, "P1UP");
    _engine->registerAction(sf::Keyboard::A, "P1LEFT");
    _engine->registerAction(sf::Keyboard::S, "P1DOWN");
    _engine->registerAction(sf::Keyboard::D, "P1RIGHT");
    _engine->registerAction(sf::Keyboard::R, "RESET_ROOMS");
    _engine->registerAction(sf::Keyboard::T, "GENERATION_STEP");

    _engine->registerAction(GameEngine::MOUSE_SCROLL_UP, "ZOOM_IN");
    _engine->registerAction(GameEngine::MOUSE_SCROLL_DOWN, "ZOOM_OUT");

    _engine->registerSystem(std::make_shared<SPhysics>(&_entities, Priority::PHYSICS));
    _engine->registerSystem(std::make_shared<SMovement>(&_entities, Priority::UPDATE));
    _engine->registerSystem(std::make_shared<SRender>(&_entities, Priority::RENDER, _window));
}

void Scene_DungeonTest::update(float dt)
{
    if (!_separatedRooms)
    {
        _separatedRooms = true;
        for (auto room1 : _entities.getEntities("Room"))
        {
            for (auto room2 : _entities.getEntities("Room"))
            {
                if (room1 == room2) continue;

                Rect room1Rect = room1->getComponent<CRectCollider>()->rect;
                Rect room2Rect = room2->getComponent<CRectCollider>()->rect;

                if (Physics::checkCollision(room1Rect, room2Rect))
                {
                    int diffs[4] = { abs(room1Rect.pos.x - (room2Rect.pos.x + room2Rect.size.x)),
                                abs((room1Rect.pos.x + room1Rect.size.x) - room2Rect.pos.x),
                                abs(room1Rect.pos.y - (room2Rect.pos.y + room2Rect.size.y)),
                                abs((room1Rect.pos.y + room1Rect.size.y) - room2Rect.pos.y) };

                    int minIndex = 0;
                    int minVal = diffs[minIndex];

                    for (int i = 1; i < 4; i++)
                    {
                        if (diffs[i] < minVal)
                        {
                            minIndex = i;
                            minVal = diffs[i];
                        }
                    }

                    int randTry = rand() % 2;
                    if (randTry == 0)
                    {
                        if (minIndex == 0) room2->getComponent<CRectCollider>()->rect.pos.x -= diffs[minIndex];
                        if (minIndex == 1) room2->getComponent<CRectCollider>()->rect.pos.x += diffs[minIndex];
                        if (minIndex == 2) room2->getComponent<CRectCollider>()->rect.pos.y -= diffs[minIndex];
                        if (minIndex == 3) room2->getComponent<CRectCollider>()->rect.pos.y += diffs[minIndex];
                    }
                    else
                    {
                        if (minIndex == 0) room1->getComponent<CRectCollider>()->rect.pos.x += diffs[minIndex];
                        if (minIndex == 1) room1->getComponent<CRectCollider>()->rect.pos.x -= diffs[minIndex];
                        if (minIndex == 2) room1->getComponent<CRectCollider>()->rect.pos.y += diffs[minIndex];
                        if (minIndex == 3) room1->getComponent<CRectCollider>()->rect.pos.y -= diffs[minIndex];
                    }

                    _separatedRooms = false;
                }
            }
        }
    }
}

void Scene_DungeonTest::sDoAction(const Action& action)
{
    if (action.name == "PHYSICS_TOGGLE" && action.type == Action::ActionType::START) GameEngine::DEBUG_MODE = !GameEngine::DEBUG_MODE;

    float scale = _camera->getComponent<CTransform>()->scale.x / _window->getSize().x;

    if (action.name == "P1UP") _camera->getComponent<CPhysicsBody>()->velocity.y = action.type == Action::ActionType::START? -200 * scale : 0;
    if (action.name == "P1LEFT") _camera->getComponent<CPhysicsBody>()->velocity.x = action.type == Action::ActionType::START ? -200 * scale : 0;
    if (action.name == "P1DOWN") _camera->getComponent<CPhysicsBody>()->velocity.y = action.type == Action::ActionType::START ? 200 * scale : 0;
    if (action.name == "P1RIGHT") _camera->getComponent<CPhysicsBody>()->velocity.x = action.type == Action::ActionType::START ? 200 * scale : 0;

    if (action.name == "ZOOM_IN") _camera->getComponent<CTransform>()->scale /= 1.05f;
    if (action.name == "ZOOM_OUT") _camera->getComponent<CTransform>()->scale *= 1.05f;

    if (action.name == "RESET_ROOMS" && action.type == Action::ActionType::START)
    {
        for (auto room : _entities.getEntities("Room"))
        {
            room->destroy();
        }
        generateRooms(100);
    }
    if (action.name == "GENERATION_STEP" && action.type == Action::ActionType::START) _separatedRooms = false;
}

void Scene_DungeonTest::generateRooms(int numRooms)
{
    for (int i = 0; i < numRooms; i++)
    {
        createRoom();
    }    
}

std::shared_ptr<Entity> Scene_DungeonTest::createRoom()
{
    auto e = _entities.addEntity("Room");
    int width = rand() % 1000 + 300;
    int height = rand() % 1000 + 300;
    int xOffset = rand() % 1000;
    int yOffset = rand() % 1000;
    e->addComponent<CTransform>(Vector2(xOffset, yOffset));
    e->addComponent<CRectCollider>(Rect(xOffset, yOffset, width, height));
    _separatedRooms = false;
    return e;
}
