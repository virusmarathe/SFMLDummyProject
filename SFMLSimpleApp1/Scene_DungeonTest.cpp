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
#include "Math/delaunator.hpp"
#include "DataStructures/Graph.h"

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
        _createRoomGraph = true;
        for (auto room1 : _entities.getEntities("Room"))
        {
            for (auto room2 : _entities.getEntities("Room"))
            {
                if (room1 == room2) continue;

                Rect room1Rect = room1->getComponent<CRectCollider>()->rect;
                Rect room2Rect = room2->getComponent<CRectCollider>()->rect;

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

                    room1->getComponent<CTransform>()->position = room1->getComponent<CRectCollider>()->rect.pos;
                    room2->getComponent<CTransform>()->position = room2->getComponent<CRectCollider>()->rect.pos;

                    _separatedRooms = false;
                    _createRoomGraph = false;
                }
            }
        }
    }
    if (_createRoomGraph)
    {
        std::vector<double> coords;
        Graph g;
        std::map<size_t, size_t> indexToIDMap;

        for (size_t i = 0; i < 15; i++) // 15 biggest rooms
        {
            _rooms[i]->addComponent<CShapeRect>(_rooms[i]->getComponent<CRectCollider>()->rect, sf::Color::Blue);
            _rooms[i]->addComponent<CText>(std::to_string(_rooms[i]->id()).c_str(), _assets->getFont("NormalUIFont"), 240, sf::Color::Red);
            Vector2 centerPos = _rooms[i]->getComponent<CRectCollider>()->rect.pos + (_rooms[i]->getComponent<CRectCollider>()->rect.size / 2.0f);
            coords.push_back(centerPos.x);
            coords.push_back(centerPos.y);
            indexToIDMap[i] = _rooms[i]->id();
            g.addNode(_rooms[i]->id());
        }

        // delaunay
        delaunator::Delaunator d(coords);

        for (size_t i = 0; i < d.triangles.size(); i += 3) 
        {
            std::shared_ptr<Entity> ent1 = _entities[indexToIDMap[d.triangles[i]]];
            std::shared_ptr<Entity> ent2 = _entities[indexToIDMap[d.triangles[i+1]]];
            std::shared_ptr<Entity> ent3 = _entities[indexToIDMap[d.triangles[i+2]]];
            Vector2 point1(ent1->getComponent<CRectCollider>()->rect.pos + (ent1->getComponent<CRectCollider>()->rect.size / 2.0f));
            Vector2 point2(ent2->getComponent<CRectCollider>()->rect.pos + (ent2->getComponent<CRectCollider>()->rect.size / 2.0f));
            Vector2 point3(ent3->getComponent<CRectCollider>()->rect.pos + (ent3->getComponent<CRectCollider>()->rect.size / 2.0f));
            g.addEdge(ent1->id(), ent2->id(), Vector2::dist(point1, point2));
            g.addEdge(ent2->id(), ent3->id(), Vector2::dist(point2, point3));
            g.addEdge(ent3->id(), ent1->id(), Vector2::dist(point3, point1));
        }

        Graph minGraph = g.getMinSpanTree();

        // Delaunay drawings
        /*for (auto const& pair : g.getNodes())
        {
            GraphNode curNode = pair.second;
            std::shared_ptr<Entity> ent = _entities[pair.first];
            Vector2 point1(ent->getComponent<CRectCollider>()->rect.pos + (ent->getComponent<CRectCollider>()->rect.size / 2.0f));

            for (auto const& edge : curNode.adjacencyList)
            {
                std::shared_ptr<Entity> ent2 = _entities[edge.node->ID];
                Vector2 point2(ent2->getComponent<CRectCollider>()->rect.pos + (ent2->getComponent<CRectCollider>()->rect.size / 2.0f));

                auto e1 = _entities.addEntity("Edge");
                e1->addComponent<CShapeLine>(point1, point2, sf::Color::Red);
            }
        }*/

        for (auto const& pair : minGraph.getNodes())
        {
            GraphNode curNode = pair.second;
            std::shared_ptr<Entity> ent = _entities[pair.first];
            Vector2 point1(ent->getComponent<CRectCollider>()->rect.pos + (ent->getComponent<CRectCollider>()->rect.size / 2.0f));

            for (auto const& edge : curNode.adjacencyList)
            {
                std::shared_ptr<Entity> ent2 = _entities[edge.node->ID];
                Vector2 point2(ent2->getComponent<CRectCollider>()->rect.pos + (ent2->getComponent<CRectCollider>()->rect.size / 2.0f));

                auto e1 = _entities.addEntity("Edge");
                e1->addComponent<CShapeLine>(point1, point2);
            }
        }
        _createRoomGraph = false;
        g.printGraph();
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
        for (auto edge : _entities.getEntities("Edge"))
        {
            edge->destroy();
        }
        generateRooms(100);
    }
}

void Scene_DungeonTest::generateRooms(int numRooms)
{
    _rooms.clear();
    std::priority_queue<std::shared_ptr<Entity>, std::vector<std::shared_ptr<Entity>>, RoomCompare> rooms;

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
}

std::shared_ptr<Entity> Scene_DungeonTest::createRoom()
{
    auto e = _entities.addEntity("Room");
    int width = rand() % 1000 + 300;
    int height = rand() % 1000 + 300;
    int xOffset = rand() % 1000;
    int yOffset = rand() % 1000;
    e->addComponent<CTransform>(Vector2((float)xOffset, (float)yOffset));
    e->addComponent<CRectCollider>(Rect((float)xOffset, (float)yOffset, (float)width, (float)height));
    return e;
}
