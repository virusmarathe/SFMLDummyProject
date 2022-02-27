#include "Scene_DungeonTest.h"
#include "Framework/GameEngine.h"
#include "Resources/Assets.h"
#include "Framework/Action.h"
#include "../Settings.h"
#include "System/SRender.h"
#include "System/SMovement.h"
#include "System/SAnimation.h"
#include "System/SPhysics.h"
#include <iostream>
#include "Math/delaunator.hpp"
#include "DataStructures/Graph.h"
#include "Framework/Primitives.h"
#include "System/SDamage.h"
#include "System/SHealthBar.h"

const float GRID_SIZE = 50.0f;

enum PhysicsLayer
{
    DEFAULT = 1,
    PLAYER = 2,
    ENEMY = 4,
    NEUTRAL = 8,
    ALL = (DEFAULT | PLAYER | ENEMY | NEUTRAL)
};

void Scene_DungeonTest::init()
{
    _assets = _engine->getAssets();

    _camera = _entities.addEntity("Camera");
    auto& transform = _camera.addComponent<CTransform>();
    transform.position = Vector2();
    transform.scale = Vector2(_window->getSize());
    _camera.addComponent<CPhysicsBody>();

    _engine->registerAction(sf::Keyboard::P, "PHYSICS_TOGGLE");
    _engine->registerAction(sf::Keyboard::Num2, "PHYSICS_SCENE");
    _engine->registerAction(sf::Keyboard::W, "P1UP");
    _engine->registerAction(sf::Keyboard::A, "P1LEFT");
    _engine->registerAction(sf::Keyboard::S, "P1DOWN");
    _engine->registerAction(sf::Keyboard::D, "P1RIGHT");
    _engine->registerAction(sf::Keyboard::LShift, "P1RUN");
    _engine->registerAction(sf::Keyboard::R, "RESET_ROOMS");
    _engine->registerAction(sf::Keyboard::F, "CAM_CHANGE");
    _engine->registerAction(GameEngine::MOUSE_SCROLL_UP, "ZOOM_IN");
    _engine->registerAction(GameEngine::MOUSE_SCROLL_DOWN, "ZOOM_OUT");
    _engine->registerAction(GameEngine::MOUSE_LEFT_DOWN, "FIRE");
    _engine->registerAction(GameEngine::MOUSE_MOVED, "FIRE_TARGET");

    _engine->registerSystem(std::make_shared<SPhysics>(&_entities, Priority::PHYSICS));
    _engine->registerSystem(std::make_shared<SDamage>(&_entities, Priority::PHYSICS + 1));
    _engine->registerSystem(std::make_shared<SHealthBar>(&_entities, Priority::PHYSICS + 1));
    _engine->registerSystem(std::make_shared<SMovement>(&_entities, Priority::UPDATE));
    _engine->registerSystem(std::make_shared<SAnimation>(&_entities, Priority::UPDATE, _assets));
    _engine->registerSystem(std::make_shared<SRender>(&_entities, Priority::RENDER, _window, _camera));

    _engine->playBGMusic("Level1BG");

    generateRooms(100);
}

const float HALLWAY_WIDTH_HALF = GRID_SIZE * 3;

void Scene_DungeonTest::preUpdate(float dt)
{
    if (!_separatedRooms)
    {
        _separatedRooms = true;
        _createRoomGraph = true;
        for (auto room1 : _entities.getEntities("Room"))
        {
            for (auto room2 : _entities.getEntities("Room"))
            {
                if (room1 == room2) continue;

                Rect room1Rect = room1.getComponent<CRectCollider>().rect;
                Rect room2Rect = room2.getComponent<CRectCollider>().rect;

                if (Physics::checkCollision(room1Rect, room2Rect))
                {
                    float diffs[4] = { abs(room1Rect.pos.x - (room2Rect.pos.x + room2Rect.size.x)),
                                abs((room1Rect.pos.x + room1Rect.size.x) - room2Rect.pos.x),
                                abs(room1Rect.pos.y - (room2Rect.pos.y + room2Rect.size.y)),
                                abs((room1Rect.pos.y + room1Rect.size.y) - room2Rect.pos.y) };

                    int minIndex = 0;
                    float minVal = diffs[minIndex];

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
                        if (minIndex == 0) room2.getComponent<CRectCollider>().rect.pos.x -= diffs[minIndex];
                        if (minIndex == 1) room2.getComponent<CRectCollider>().rect.pos.x += diffs[minIndex];
                        if (minIndex == 2) room2.getComponent<CRectCollider>().rect.pos.y -= diffs[minIndex];
                        if (minIndex == 3) room2.getComponent<CRectCollider>().rect.pos.y += diffs[minIndex];
                    }
                    else
                    {
                        if (minIndex == 0) room1.getComponent<CRectCollider>().rect.pos.x += diffs[minIndex];
                        if (minIndex == 1) room1.getComponent<CRectCollider>().rect.pos.x -= diffs[minIndex];
                        if (minIndex == 2) room1.getComponent<CRectCollider>().rect.pos.y += diffs[minIndex];
                        if (minIndex == 3) room1.getComponent<CRectCollider>().rect.pos.y -= diffs[minIndex];
                    }

                    room1.getComponent<CTransform>().position = room1.getComponent<CRectCollider>().rect.pos;
                    room2.getComponent<CTransform>().position = room2.getComponent<CRectCollider>().rect.pos;

                    _separatedRooms = false;
                    _createRoomGraph = false;
                }
            }
        }
    }

    if (_createBorder)
    {
        Vector2 normal, contactPoint;
        float tHitNear, tHitFar;
        for (auto room1 : _entities.getEntities("Room"))
        {
            Rect room1Rect = room1.getComponent<CRectCollider>().rect;
            Vector2 topLeft = room1Rect.pos - Vector2(0, 1); // offset by 1 pixel up to check collisions
            Vector2 bottomLeft = Vector2(room1Rect.pos.x, room1Rect.pos.y + room1Rect.size.y + 1);
            Vector2 leftTop = room1Rect.pos - Vector2(1,0);
            Vector2 rightTop = room1Rect.pos + Vector2(room1Rect.size.x + 1, 0);
            Vector2 right(room1Rect.size.x, 0);
            Vector2 down(0, room1Rect.size.y);
            std::map<float, float> topFarValues, bottomFarValues, leftFarValues, rightFarValues;
            std::set<float> topOrderedList, bottomOrderedList, leftOrderedList, rightOrderedList;

            for (auto room2 : _entities.getEntities("Room"))
            {
                if (room1 == room2) continue;

                Rect room2Rect = room2.getComponent<CRectCollider>().rect;
                if (Physics::checkCollision(topLeft, right, room2Rect, contactPoint, normal, tHitNear, tHitFar))
                {
                    topOrderedList.insert(tHitNear);
                    topFarValues[tHitNear] = tHitFar;
                }

                if (Physics::checkCollision(bottomLeft, right, room2Rect, contactPoint, normal, tHitNear, tHitFar))
                {
                    bottomOrderedList.insert(tHitNear);
                    bottomFarValues[tHitNear] = tHitFar;
                }

                if (Physics::checkCollision(leftTop, down, room2Rect, contactPoint, normal, tHitNear, tHitFar))
                {
                    leftOrderedList.insert(tHitNear);
                    leftFarValues[tHitNear] = tHitFar;
                }

                if (Physics::checkCollision(rightTop, down, room2Rect, contactPoint, normal, tHitNear, tHitFar))
                {
                    rightOrderedList.insert(tHitNear);
                    rightFarValues[tHitNear] = tHitFar;
                }
            }

            // top border
            float startX = topLeft.x;
            for (auto const& nearVal : topOrderedList)
            {
                Vector2 nearPoint(topLeft + (right * nearVal));
                Vector2 farPoint(topLeft + (right * topFarValues[nearVal]));
                if (startX < nearPoint.x && nearVal >= 0)
                {
                    Vector2 startPoint(startX, topLeft.y + 1);
                    createWall(Rect(startPoint, Vector2(nearPoint.x - startPoint.x, GRID_SIZE)), "Wall");
                }
                
                if (farPoint.x > startX)
                {
                    startX = farPoint.x;
                }
                if (topFarValues[nearVal] > 1) break;
            }

            if (startX < topLeft.x + room1Rect.size.x)
            {
                Vector2 startPoint(startX, topLeft.y + 1);
                Vector2 endPoint(topLeft.x + room1Rect.size.x, topLeft.y);
                createWall(Rect(startPoint, Vector2(endPoint.x - startPoint.x, GRID_SIZE)), "Wall");
            }

            // bottom border
            startX = bottomLeft.x;
            for (auto const& nearVal : bottomOrderedList)
            {
                Vector2 nearPoint(bottomLeft + (right * nearVal));
                Vector2 farPoint(bottomLeft + (right * bottomFarValues[nearVal]));
                if (startX < nearPoint.x && nearVal >= 0)
                {
                    Vector2 startPoint(startX, bottomLeft.y - GRID_SIZE - 1);
                    createWall(Rect(startPoint, Vector2(nearPoint.x - startPoint.x, GRID_SIZE)), "Wall");
                }

                if (farPoint.x > startX)
                {
                    startX = farPoint.x;
                }
                if (bottomFarValues[nearVal] > 1) break;
            }

            if (startX < bottomLeft.x + room1Rect.size.x)
            {
                Vector2 startPoint(startX, bottomLeft.y - GRID_SIZE - 1);
                Vector2 endPoint(bottomLeft.x + room1Rect.size.x, bottomLeft.y);
                createWall(Rect(startPoint, Vector2(endPoint.x - startPoint.x, GRID_SIZE)), "Wall");
            }

            // left border
            float startY = leftTop.y;
            for (auto const& nearVal : leftOrderedList)
            {
                Vector2 nearPoint(leftTop + (down * nearVal));
                Vector2 farPoint(leftTop + (down * leftFarValues[nearVal]));
                if (startY < nearPoint.y && nearVal >= 0)
                {
                    Vector2 startPoint(leftTop.x + 1, startY);
                    createWall(Rect(startPoint, Vector2(GRID_SIZE, nearPoint.y - startPoint.y)), "Wall");
                }

                if (farPoint.y > startY)
                {
                    startY = farPoint.y;
                }
                if (leftFarValues[nearVal] > 1) break;
            }

            if (startY < leftTop.y + room1Rect.size.y)
            {
                Vector2 startPoint(leftTop.x + 1, startY);
                Vector2 endPoint(leftTop.x, leftTop.y + room1Rect.size.y);
                createWall(Rect(startPoint, Vector2(GRID_SIZE, endPoint.y - startPoint.y)), "Wall");
            }

            // right border
            startY = rightTop.y;
            for (auto const& nearVal : rightOrderedList)
            {
                Vector2 nearPoint(rightTop + (down * nearVal));
                Vector2 farPoint(rightTop + (down * rightFarValues[nearVal]));
                if (startY < nearPoint.y && nearVal >= 0)
                {
                    Vector2 startPoint(rightTop.x - GRID_SIZE - 1, startY);
                    createWall(Rect(startPoint, Vector2(GRID_SIZE, nearPoint.y - startPoint.y)), "Wall");
                }

                if (farPoint.y > startY)
                {
                    startY = farPoint.y;
                }
                if (rightFarValues[nearVal] > 1) break;
            }

            if (startY < rightTop.y + room1Rect.size.y)
            {
                Vector2 startPoint(rightTop.x - GRID_SIZE - 1, startY);
                Vector2 endPoint(rightTop.x, rightTop.y + room1Rect.size.y);
                createWall(Rect(startPoint, Vector2(GRID_SIZE, endPoint.y - startPoint.y)), "Wall");
            }
        }
        _createBorder = false;
        for (auto room : _entities.getEntities("Room"))
        {
            Rect rect = room.getComponent<CRectCollider>().rect;
            auto& groundSpriteComp = room.addComponent<CSprite>();
            groundSpriteComp.sprite = sf::Sprite(_assets->getTexture("Ground"));
            groundSpriteComp.sprite.setTextureRect(sf::IntRect(0, 0, (int)rect.size.x, (int)rect.size.y));

            room.removeComponent<CRectCollider>();
            room.removeComponent<CShapeRect>();
        }

        _player1Entity.destroy();
        _player1Entity = spawnPlayer();
        _player1Entity.getComponent<CTransform>().position = Vector2(_rooms[0].getComponent<CTransform>().position) + Vector2(300, 300);

        for (int i = 0; i < 5; i++)
        {
            auto barrel = Primitives::ScaledSprite(Rect(_player1Entity.getComponent<CTransform>().position + Vector2((float)i * 100, 100), Vector2(100, 100)), _assets->getTexture("Barrel"), true, "Destructable");
            auto& healthComp = barrel.addComponent<CHealth>();
            healthComp.health = 100.0f;
            healthComp.maxHealth = 100.0f;
            healthComp.hitLayer = PhysicsLayer::ALL;
            healthComp.background.setSize(sf::Vector2f(100, 10));
            healthComp.background.setFillColor(sf::Color::Red);
            healthComp.foreground.setSize(sf::Vector2f(100, 10));
            healthComp.foreground.setFillColor(sf::Color::Green);
        }
    }

    if (_createRoomGraph)
    {
        std::vector<double> coords;
        Graph g;
        std::map<size_t, size_t> indexToIDMap;

        for (size_t i = 0; i < 15; i++) // 15 biggest rooms
        {
            auto& rectShape = _rooms[i].addComponent<CShapeRect>();
            rectShape.rectShape = sf::RectangleShape(sf::Vector2f(_rooms[i].getComponent<CRectCollider>().rect.size.x, _rooms[i].getComponent<CRectCollider>().rect.size.y));
            rectShape.color = sf::Color::Blue;
            //_rooms[i]->addComponent<CText>(std::to_string(_rooms[i]->id()).c_str(), _assets->getFont("NormalUIFont"), 240, sf::Color::Red);
            Vector2 centerPos = _rooms[i].getComponent<CRectCollider>().rect.pos + (_rooms[i].getComponent<CRectCollider>().rect.size / 2.0f);
            coords.push_back(centerPos.x);
            coords.push_back(centerPos.y);
            indexToIDMap[i] = _rooms[i].id;
            g.addNode(_rooms[i].id);
        }

        // delaunay
        delaunator::Delaunator d(coords);

        for (size_t i = 0; i < d.triangles.size(); i += 3) 
        {   
            Entity ent1 = _entities[indexToIDMap[d.triangles[i]]];
            Entity ent2 = _entities[indexToIDMap[d.triangles[i+1]]];
            Entity ent3 = _entities[indexToIDMap[d.triangles[i+2]]];
            Vector2 point1(ent1.getComponent<CRectCollider>().rect.pos + (ent1.getComponent<CRectCollider>().rect.size / 2.0f));
            Vector2 point2(ent2.getComponent<CRectCollider>().rect.pos + (ent2.getComponent<CRectCollider>().rect.size / 2.0f));
            Vector2 point3(ent3.getComponent<CRectCollider>().rect.pos + (ent3.getComponent<CRectCollider>().rect.size / 2.0f));
            g.addEdge(ent1.id, ent2.id, Vector2::dist(point1, point2));
            g.addEdge(ent2.id, ent3.id, Vector2::dist(point2, point3));
            g.addEdge(ent3.id, ent1.id, Vector2::dist(point3, point1));
        }

        Graph minGraph = g.getMinSpanTree();

        // randomly add some edges back in for cycles
        for (auto const& pair : g.getNodes())
        {
            GraphNode curNode = pair.second;
            for (auto const& edge : curNode.adjacencyList)
            {
                if (rand() % 100 < 10) // 10% chance to add a edge back in
                {
                    minGraph.addEdge(curNode.ID, edge.node->ID, edge.cost);
                }
            }
        }

        // Delaunay drawings
        /*for (auto const& pair : g.getNodes())
        {
            GraphNode curNode = pair.second;
            std::shared_ptr<Entity> ent = _entities[pair.first];
            Vector2 point1(ent.getComponent<CRectCollider>()->rect.pos + (ent.getComponent<CRectCollider>()->rect.size / 2.0f));

            for (auto const& edge : curNode.adjacencyList)
            {
                std::shared_ptr<Entity> ent2 = _entities[edge.node->ID];
                Vector2 point2(ent2.getComponent<CRectCollider>()->rect.pos + (ent2.getComponent<CRectCollider>()->rect.size / 2.0f));

                auto e1 = _entities.addEntity("Edge");
                e1->addComponent<CShapeLine>(point1, point2, sf::Color::Red);
            }
        }*/

        std::vector<Vector2> hallways;

        for (auto const& pair : minGraph.getNodes())
        {
            GraphNode curNode = pair.second;
            Entity ent = _entities[pair.first];
            Vector2 point1(ent.getComponent<CRectCollider>().rect.pos + (ent.getComponent<CRectCollider>().rect.size / 2.0f));
            point1.x = ((int)(point1.x / (GRID_SIZE * 2))) * (GRID_SIZE * 2);
            point1.y = ((int)(point1.y / (GRID_SIZE * 2))) * (GRID_SIZE * 2);

            for (auto const& edge : curNode.adjacencyList)
            {
                Entity ent2 = _entities[edge.node->ID];
                Vector2 point2(ent2.getComponent<CRectCollider>().rect.pos + (ent2.getComponent<CRectCollider>().rect.size / 2.0f));
                point2.x = ((int)(point2.x / (GRID_SIZE * 2))) * (GRID_SIZE * 2);
                point2.y = ((int)(point2.y / (GRID_SIZE * 2))) * (GRID_SIZE * 2);

                if (abs(point1.x - point2.x) < (ent2.getComponent<CRectCollider>().rect.size.x / 2.0f + ent.getComponent<CRectCollider>().rect.size.x / 2.0f))
                {
                    auto e1 = _entities.addEntity("Room");
                    float xVal = (point1.x + point2.x) / 2.0f;
                    float minY = point1.y < point2.y ? point1.y : point2.y;
                    float height = abs(point2.y - point1.y);
                    auto& transform = e1.addComponent<CTransform>();
                    transform.position = Vector2(xVal - HALLWAY_WIDTH_HALF, minY);
                    auto& rectCollider = e1.addComponent<CRectCollider>();
                    rectCollider.rect = Rect(xVal - HALLWAY_WIDTH_HALF, minY, HALLWAY_WIDTH_HALF * 2, height);
                    auto& shapeRect = e1.addComponent<CShapeRect>();
                    shapeRect.rectShape = sf::RectangleShape(sf::Vector2f(HALLWAY_WIDTH_HALF * 2, height));
                    shapeRect.color = sf::Color::Red;
                    hallways.push_back(Vector2(xVal, point1.y));
                    hallways.push_back(Vector2(xVal, point2.y));
                }
                else if (abs(point1.y - point2.y) < (ent2.getComponent<CRectCollider>().rect.size.y / 2.0f + ent.getComponent<CRectCollider>().rect.size.y / 2.0f))
                {
                    auto e1 = _entities.addEntity("Room");
                    float yVal = (point1.y + point2.y) / 2.0f;
                    float minX = point1.x < point2.x ? point1.x : point2.x;
                    float width = abs(point2.x - point1.x);
                    auto& transform = e1.addComponent<CTransform>();
                    transform.position = Vector2(minX, yVal - HALLWAY_WIDTH_HALF);
                    auto& rectCollider = e1.addComponent<CRectCollider>();
                    rectCollider.rect = Rect(minX, yVal - HALLWAY_WIDTH_HALF, width, HALLWAY_WIDTH_HALF * 2);
                    auto& shapeRect = e1.addComponent<CShapeRect>();
                    shapeRect.rectShape = sf::RectangleShape(sf::Vector2f(width, HALLWAY_WIDTH_HALF * 2));
                    shapeRect.color = sf::Color::Red;
                    hallways.push_back(Vector2(point1.x, yVal));
                    hallways.push_back(Vector2(point2.x, yVal));
                }
                else
                {
                    auto e1 = _entities.addEntity("Room");
                    float xVal = point1.x;
                    float minY = point1.y < point2.y ? point1.y - HALLWAY_WIDTH_HALF : point2.y - HALLWAY_WIDTH_HALF;
                    float height = abs(point2.y - point1.y) + HALLWAY_WIDTH_HALF * 2;
                    auto& transform = e1.addComponent<CTransform>();
                    transform.position = Vector2(xVal - HALLWAY_WIDTH_HALF, minY);
                    auto& rectCollider = e1.addComponent<CRectCollider>();
                    rectCollider.rect = Rect(xVal - HALLWAY_WIDTH_HALF, minY, HALLWAY_WIDTH_HALF * 2, height);
                    auto& shapeRect = e1.addComponent<CShapeRect>();
                    shapeRect.rectShape = sf::RectangleShape(sf::Vector2f(HALLWAY_WIDTH_HALF * 2, height));
                    shapeRect.color = sf::Color::Red;
                    hallways.push_back(point1);
                    hallways.push_back(Vector2(point1.x, point2.y));

                    auto e2 = _entities.addEntity("Room");
                    float yVal = point1.y;
                    float minX = point1.x < point2.x ? point1.x - HALLWAY_WIDTH_HALF : point2.x - HALLWAY_WIDTH_HALF;
                    float width = abs(point2.x - point1.x) + HALLWAY_WIDTH_HALF * 2;
                    auto& transform2 = e2.addComponent<CTransform>();
                    transform2.position = Vector2(minX, yVal - HALLWAY_WIDTH_HALF);
                    auto& rectCollider2 = e2.addComponent<CRectCollider>();
                    rectCollider2.rect = Rect(minX, yVal - HALLWAY_WIDTH_HALF, width, HALLWAY_WIDTH_HALF * 2);
                    auto& shapeRect2 = e2.addComponent<CShapeRect>();
                    shapeRect2.rectShape = sf::RectangleShape(sf::Vector2f(width, HALLWAY_WIDTH_HALF * 2));
                    shapeRect2.color = sf::Color::Red;
                    hallways.push_back(point1);
                    hallways.push_back(Vector2(point2.x, point1.y));
                }

                //auto e1 = _entities.addEntity("Edge");
                //e1->addComponent<CShapeLine>(point1, point2);
            }
        }

        // enable rooms that overlap with hallways
        for (size_t i = 0; i < hallways.size(); i+=2)
        {
            for (auto room : _entities.getEntities("Room"))
            {
                if (room.hasComponent<CShapeRect>()) continue;

                Rect roomRect = room.getComponent<CRectCollider>().rect;
                Vector2 contactPoint, normal;
                float hitTime;
                if (Physics::checkCollision(hallways[i], hallways[i+1] - hallways[i], roomRect, contactPoint, normal, hitTime))
                {
                    auto& shapeRect = room.addComponent<CShapeRect>();
                    shapeRect.rectShape = sf::RectangleShape(sf::Vector2f(room.getComponent<CRectCollider>().rect.size.x, room.getComponent<CRectCollider>().rect.size.y));
                    shapeRect.color = sf::Color::Red;
                }
            }
        }

        for (auto& room : _rooms)
        {
            if (!room.hasComponent<CShapeRect>())
            {
                room.destroy();
            }
        }

        _createRoomGraph = false;
        _createBorder = true;
    }

    if (!_freeCam && _player1Entity.id < MAX_ENTITIES)
    {
        _camera.getComponent<CTransform>().position = _player1Entity.getComponent<CTransform>().position;
    }

    if (_firing)
    {
        _fireCooldownTimer += dt;
        if (_fireCooldownTimer >= 0.1f)
        {
            for (int i = 0; i < 10; i++)
            {
                fireBullet(_firePos + Vector2((float)(rand()%100), (float)(rand()%100)));
            }
            _fireCooldownTimer = 0;
        }
    }
    else _fireCooldownTimer = 0.25f;
}

void Scene_DungeonTest::sDoAction(const Action& action)
{
    if (action.name == "PHYSICS_TOGGLE" && action.type == Action::ActionType::START) GameEngine::DEBUG_MODE = !GameEngine::DEBUG_MODE;
    if (action.name == "CAM_CHANGE" && action.type == Action::ActionType::START) _freeCam = !_freeCam;
    if (action.name == "PHYSICS_SCENE" && action.type == Action::ActionType::START) _engine->changeScene("PhysicsTest");

    float scale = _camera.getComponent<CTransform>().scale.x / _window->getSize().x;

    if (_freeCam)
    {
        if (action.name == "P1UP") _camera.getComponent<CPhysicsBody>().velocity.y = action.type == Action::ActionType::START ? -200 * scale : 0;
        if (action.name == "P1LEFT") _camera.getComponent<CPhysicsBody>().velocity.x = action.type == Action::ActionType::START ? -200 * scale : 0;
        if (action.name == "P1DOWN") _camera.getComponent<CPhysicsBody>().velocity.y = action.type == Action::ActionType::START ? 200 * scale : 0;
        if (action.name == "P1RIGHT") _camera.getComponent<CPhysicsBody>().velocity.x = action.type == Action::ActionType::START ? 200 * scale : 0;
    }
    else
    {
        if (_player1Entity.id < MAX_ENTITIES)
        {
            if (action.name == "P1UP") _player1Entity.getComponent<CInput>().up = action.type == Action::ActionType::START;
            if (action.name == "P1LEFT") _player1Entity.getComponent<CInput>().left = action.type == Action::ActionType::START;
            if (action.name == "P1DOWN") _player1Entity.getComponent<CInput>().down = action.type == Action::ActionType::START;
            if (action.name == "P1RIGHT") _player1Entity.getComponent<CInput>().right = action.type == Action::ActionType::START;
            if (action.name == "P1RUN") _player1Entity.getComponent<CInput>().run = action.type == Action::ActionType::START;
            if (action.name == "FIRE") _firing = action.type == Action::ActionType::START;
            if (action.name == "FIRE_TARGET") _firePos = action.pos;
        }
    }

    if (action.name == "ZOOM_IN") _camera.getComponent<CTransform>().scale /= 1.05f;
    if (action.name == "ZOOM_OUT") _camera.getComponent<CTransform>().scale *= 1.05f;

    if (action.name == "RESET_ROOMS" && action.type == Action::ActionType::START)
    {
        for (auto ent : _entities.getEntities())
        {
            if (ent.tag() == "Room" || ent.tag() == "Wall" || ent.tag() == "Destructable")
            {
                ent.destroy();
            }
        }
        generateRooms(100);
    }

    sInput();
}

void Scene_DungeonTest::postUpdate(float dt)
{
    sHandleCollision();
}

void Scene_DungeonTest::generateRooms(int numRooms)
{
    _rooms.clear();
    std::priority_queue<Entity, std::vector<Entity>, RoomCompare> rooms;

    for (int i = 0; i < numRooms; i++)
    {
        rooms.push(createRoom());
    }

    while (rooms.size() != 0)
    {
        _rooms.push_back(rooms.top());
        rooms.pop();
    }

    _separatedRooms = false;
    _createRoomGraph = false;
    _createBorder = false;
}

Entity Scene_DungeonTest::createRoom()
{
    auto e = _entities.addEntity("Room");
    int width = rand() % ((int)GRID_SIZE * 20) + ((int)GRID_SIZE * 6);
    width = (width / (int)GRID_SIZE) * ((int)GRID_SIZE);
    int height = rand() % ((int)GRID_SIZE * 20) + ((int)GRID_SIZE * 6);
    height = (height / (int)GRID_SIZE) * ((int)GRID_SIZE);
    int xOffset = rand() % ((int)GRID_SIZE * 20);
    xOffset = (xOffset / (int)GRID_SIZE) * ((int)GRID_SIZE);
    int yOffset = rand() % ((int)GRID_SIZE * 20);
    yOffset = (yOffset / (int)GRID_SIZE) * ((int)GRID_SIZE);

    auto& transform = e.addComponent<CTransform>();
    transform.position = Vector2((float)xOffset, (float)yOffset);
    auto& rectCollider = e.addComponent<CRectCollider>();
    rectCollider.rect = Rect((float)xOffset, (float)yOffset, (float)width, (float)height);
    return e;
}

void Scene_DungeonTest::createWall(Rect wallRect, std::string assetName)
{
    Entity wall = _entities.addEntity("Wall");
    auto& transform = wall.addComponent<CTransform>();
    transform.position = wallRect.pos;
    auto& rectCollider = wall.addComponent<CRectCollider>();
    rectCollider.rect = wallRect;
    auto& spriteComp = wall.addComponent<CSprite>();
    spriteComp.sprite = sf::Sprite(_assets->getTexture(assetName));
    sf::IntRect texRect = spriteComp.sprite.getTextureRect();
    Vector2 scale(GRID_SIZE / texRect.width, GRID_SIZE / texRect.height);
    spriteComp.sprite.setScale(scale.x, scale.y);
    spriteComp.sprite.setTextureRect(sf::IntRect(0, 0, (int)(wallRect.size.x / scale.x), (int)(wallRect.size.y / scale.y)));
}

Entity Scene_DungeonTest::spawnPlayer()
{
    auto player = _entities.addEntity("Player");
    auto& transform = player.addComponent<CTransform>();
    transform.position = Vector2(200, 200);
    auto& sprite = player.addComponent<CSprite>();
    sprite.sprite = sf::Sprite(_assets->getTexture("ArcherWalk"));
    sprite.sprite.setScale(0.25f, 0.25f);
    auto& anim = player.addComponent<CAnimation>();
    anim.name = "ArcherWalk";
    auto& rectCollider = player.addComponent<CRectCollider>();
    rectCollider.rect = Rect(0, 0, 70, 80);
    rectCollider.offset = Vector2(20, 5);
    rectCollider.collisionLayer = PLAYER;
    auto& physics = player.addComponent<CPhysicsBody>();
    physics.collidesWith = DEFAULT | NEUTRAL | ENEMY;
    player.addComponent<CInput>();
    auto& animator = player.addComponent<CPhysicsAnimator>();
    animator.idleAnimName = "ArcherIdle";
    animator.movingAnimName = "ArcherWalk";
    animator.fastMovingAnimName = "ArcherRun";
    animator.velThreshold = Settings::ARCHER_SPEED;

    return player;
}

void Scene_DungeonTest::fireBullet(Vector2 mouseLocation)
{
    auto bullet = _entities.addEntity("Bullet");
    Rect playerRect = _player1Entity.getComponent<CRectCollider>().rect;
    Vector2 pos = _player1Entity.getComponent<CTransform>().position + (playerRect.size / 2.0f);
    auto& transform = bullet.addComponent<CTransform>();
    transform.position = pos;
    Vector2 vel = (mouseLocation - pos);
    vel.normalize();
    vel *= Settings::BULLET_SPEED;
    auto& physics = bullet.addComponent<CPhysicsBody>();
    physics.velocity = vel;
    physics.collidesWith = DEFAULT | NEUTRAL | ENEMY;
    auto& sprite = bullet.addComponent<CSprite>();
    sprite.sprite = sf::Sprite(_assets->getTexture("Ball"));
    sprite.sprite.setScale(0.25f, 0.25f);
    Rect box = Rect(sprite.sprite.getLocalBounds());
    box.size *= 0.25f;
    auto& rectCollider = bullet.addComponent<CRectCollider>();
    rectCollider.rect = box;
    rectCollider.isTrigger = true;
    rectCollider.collisionLayer = PLAYER;
    auto& damage = bullet.addComponent<CDamage>();
    damage.damage = 10.0f;
    damage.layer = PLAYER;
}

void Scene_DungeonTest::sInput()
{
    for (auto ent : _entities.getEntities("Player"))
    {
        if (ent.hasComponent<CInput>() && ent.hasComponent<CPhysicsBody>())
        {
            auto& input = ent.getComponent<CInput>();
            Vector2 vel;

            if (input.up)     vel.y -= Settings::ARCHER_SPEED;
            if (input.down)   vel.y += Settings::ARCHER_SPEED;
            if (input.left)   vel.x -= Settings::ARCHER_SPEED;
            if (input.right)  vel.x += Settings::ARCHER_SPEED;

            if (input.run) vel *= 2;

            ent.getComponent<CPhysicsBody>().velocity = vel;
        }
    }
}

void Scene_DungeonTest::sHandleCollision()
{
    for (auto ent : _entities.getEntities("CollisionEvent"))
    {
        auto& cEvent = ent.getComponent<CCollisionEvent>();
        Entity ent1 = _entities[cEvent.ent1];
        Entity ent2 = _entities[cEvent.ent2];
        // handle ball collision with something
        if (ent1.tag() == "Bullet")
        {
            if (ent2.tag() == "Wall" || ent2.tag() == "Destructable")
            {
                float distSqr = (_player1Entity.getComponent<CTransform>().position - ent1.getComponent<CTransform>().position).magnitudeSqr();
                ent1.getComponent<CRectCollider>().enabled = false;
                ent1.destroy();
                if (distSqr < 100000.0f) _engine->playSound("WallImpact");
            }
        }
    }
}
