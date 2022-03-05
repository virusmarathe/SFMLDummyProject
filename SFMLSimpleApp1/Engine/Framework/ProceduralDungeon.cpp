#include "ProceduralDungeon.h"
#include "Physics/Physics.h"
#include "Math/delaunator.hpp"
#include "Framework/Primitives.h"

void ProceduralDungeon::generateDungeon(int numRooms, float gridSize)
{
    _gridSize = gridSize;
    _hallwayWidthHalf = gridSize * 3;
    _allRooms.clear();
    _finalRooms.clear();
    _hallways.clear();
    _walls.clear();

    std::priority_queue<Rect, std::vector<Rect>, std::less<Rect>> rooms;

    // generate room list ordered by area
    for (int i = 0; i < numRooms; i++)
    {
        rooms.push(createRoomRect());
    }

    while (rooms.size() != 0)
    {
        _allRooms.push_back(rooms.top());
        rooms.pop();
    }

    separateRooms();
    createRoomGraph();
    createHallways();
    addSmallRooms();
    createWallRects();
}

Rect ProceduralDungeon::createRoomRect()
{
    int width = rand() % ((int)_gridSize * 20) + ((int)_gridSize * 6);
    width = (width / (int)_gridSize) * ((int)_gridSize);
    int height = rand() % ((int)_gridSize * 20) + ((int)_gridSize * 6);
    height = (height / (int)_gridSize) * ((int)_gridSize);
    int xOffset = rand() % ((int)_gridSize * 20);
    xOffset = (xOffset / (int)_gridSize) * ((int)_gridSize);
    int yOffset = rand() % ((int)_gridSize * 20);
    yOffset = (yOffset / (int)_gridSize) * ((int)_gridSize);

    return Rect((float)xOffset, (float)yOffset, (float)width, (float)height);
}

void ProceduralDungeon::separateRooms()
{
    bool allSeparated = false;

    while (!allSeparated)
    {
        allSeparated = true;
        for (auto& room1Rect : _allRooms)
        {
            for (auto& room2Rect : _allRooms)
            {
                if (room1Rect == room2Rect) continue;

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
                        if (minIndex == 0) room2Rect.pos.x -= diffs[minIndex];
                        if (minIndex == 1) room2Rect.pos.x += diffs[minIndex];
                        if (minIndex == 2) room2Rect.pos.y -= diffs[minIndex];
                        if (minIndex == 3) room2Rect.pos.y += diffs[minIndex];
                    }
                    else
                    {
                        if (minIndex == 0) room1Rect.pos.x += diffs[minIndex];
                        if (minIndex == 1) room1Rect.pos.x -= diffs[minIndex];
                        if (minIndex == 2) room1Rect.pos.y += diffs[minIndex];
                        if (minIndex == 3) room1Rect.pos.y -= diffs[minIndex];
                    }

                    allSeparated = false; // if there was a physics collision rooms aren't separated yet
                }
            }
        }
    }    
}

int numMainRooms = 15;

void ProceduralDungeon::createRoomGraph()
{
    std::vector<double> coords;
    Graph g;
    std::map<size_t, size_t> indexToIDMap;

    for (size_t i = 0; i < numMainRooms; i++) // 15 biggest rooms
    {
        coords.push_back(_allRooms[i].center().x);
        coords.push_back(_allRooms[i].center().y);
        g.addNode(i);
        _finalRooms.push_back(_allRooms[i]);
    }

    // delaunay
    delaunator::Delaunator d(coords);

    for (size_t i = 0; i < d.triangles.size(); i += 3)
    {
        size_t index1 = d.triangles[i];
        size_t index2 = d.triangles[i + 1];
        size_t index3 = d.triangles[i + 2];
        g.addEdge(index1, index2, Vector2::dist(_allRooms[index1].center(), _allRooms[index2].center()));
        g.addEdge(index2, index3, Vector2::dist(_allRooms[index2].center(), _allRooms[index3].center()));
        g.addEdge(index3, index1, Vector2::dist(_allRooms[index3].center(), _allRooms[index1].center()));
    }

    _minGraph = g.getMinSpanTree();

    // randomly add some edges back in for cycles
    for (auto const& pair : g.getNodes())
    {
        GraphNode curNode = pair.second;
        for (auto const& edge : curNode.adjacencyList)
        {
            if (rand() % 100 < 10) // 10% chance to add a edge back in
            {
                _minGraph.addEdge(curNode.ID, edge.node->ID, edge.cost);
            }
        }
    }
}

void ProceduralDungeon::createHallways()
{
    for (auto const& pair : _minGraph.getNodes())
    {
        GraphNode curNode = pair.second;
        Rect rect1 = _allRooms[pair.first];
        Vector2 point1 = rect1.center();
        point1.x = ((int)(point1.x / (_gridSize * 2))) * (_gridSize * 2);
        point1.y = ((int)(point1.y / (_gridSize * 2))) * (_gridSize * 2);

        for (auto const& edge : curNode.adjacencyList)
        {
            Rect rect2 = _allRooms[edge.node->ID];
            Vector2 point2 = rect2.center();
            point2.x = ((int)(point2.x / (_gridSize * 2))) * (_gridSize * 2);
            point2.y = ((int)(point2.y / (_gridSize * 2))) * (_gridSize * 2);

            if (abs(point1.x - point2.x) < (rect2.size.x / 2.0f + rect1.size.x / 2.0f))
            {
                float xVal = (point1.x + point2.x) / 2.0f;
                float minY = point1.y < point2.y ? point1.y : point2.y;
                float height = abs(point2.y - point1.y);
                _finalRooms.push_back(Rect(Vector2(xVal - _hallwayWidthHalf, minY), Vector2(_hallwayWidthHalf * 2, height)));
                _hallways.push_back(Vector2(xVal, point1.y));
                _hallways.push_back(Vector2(xVal, point2.y));
            }
            else if (abs(point1.y - point2.y) < (rect2.size.y / 2.0f + rect1.size.y / 2.0f))
            {
                float yVal = (point1.y + point2.y) / 2.0f;
                float minX = point1.x < point2.x ? point1.x : point2.x;
                float width = abs(point2.x - point1.x);
                _finalRooms.push_back(Rect(Vector2(minX, yVal - _hallwayWidthHalf), Vector2(width, _hallwayWidthHalf * 2)));
                _hallways.push_back(Vector2(point1.x, yVal));
                _hallways.push_back(Vector2(point2.x, yVal));
            }
            else
            {
                float xVal = point1.x;
                float minY = point1.y < point2.y ? point1.y - _hallwayWidthHalf : point2.y - _hallwayWidthHalf;
                float height = abs(point2.y - point1.y) + _hallwayWidthHalf * 2;
                _finalRooms.push_back(Rect(Vector2(xVal - _hallwayWidthHalf, minY), Vector2(_hallwayWidthHalf * 2, height)));
                _hallways.push_back(point1);
                _hallways.push_back(Vector2(point1.x, point2.y));

                float yVal = point1.y;
                float minX = point1.x < point2.x ? point1.x - _hallwayWidthHalf : point2.x - _hallwayWidthHalf;
                float width = abs(point2.x - point1.x) + _hallwayWidthHalf * 2;
                _finalRooms.push_back(Rect(Vector2(minX, yVal - _hallwayWidthHalf), Vector2(width, _hallwayWidthHalf * 2)));
                _hallways.push_back(point1);
                _hallways.push_back(Vector2(point2.x, point1.y));
            }
        }
    }
}

void ProceduralDungeon::addSmallRooms()
{
    // enable rooms that overlap with hallways
    for (size_t i = 0; i < _hallways.size(); i += 2)
    {
        for (size_t j = numMainRooms; j < _allRooms.size(); j++)
        {
            Vector2 contactPoint, normal;
            float hitTime;
            if (Physics::checkCollision(_hallways[i], _hallways[i + 1] - _hallways[i], _allRooms[j], contactPoint, normal, hitTime))
            {
                _finalRooms.push_back(_allRooms[j]);
            }
        }
    }
}

void ProceduralDungeon::createWallRects()
{
    Vector2 normal, contactPoint;
    float tHitNear, tHitFar;
    for (auto& room1Rect : _finalRooms)
    {
        Vector2 topLeft = room1Rect.pos - Vector2(0, 1); // offset by 1 pixel up to check collisions
        Vector2 bottomLeft = Vector2(room1Rect.pos.x, room1Rect.pos.y + room1Rect.size.y + 1);
        Vector2 leftTop = room1Rect.pos - Vector2(1,0);
        Vector2 rightTop = room1Rect.pos + Vector2(room1Rect.size.x + 1, 0);
        Vector2 right(room1Rect.size.x, 0);
        Vector2 down(0, room1Rect.size.y);
        std::map<float, float> topFarValues, bottomFarValues, leftFarValues, rightFarValues;
        std::set<float> topOrderedList, bottomOrderedList, leftOrderedList, rightOrderedList;

        for (auto& room2Rect : _finalRooms)
        {
            if (room1Rect == room2Rect) continue;

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
                _walls.push_back(Rect(startPoint, Vector2(nearPoint.x - startPoint.x, _gridSize)));
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
            _walls.push_back(Rect(startPoint, Vector2(endPoint.x - startPoint.x, _gridSize)));
        }

        // bottom border
        startX = bottomLeft.x;
        for (auto const& nearVal : bottomOrderedList)
        {
            Vector2 nearPoint(bottomLeft + (right * nearVal));
            Vector2 farPoint(bottomLeft + (right * bottomFarValues[nearVal]));
            if (startX < nearPoint.x && nearVal >= 0)
            {
                Vector2 startPoint(startX, bottomLeft.y - _gridSize - 1);
                _walls.push_back(Rect(startPoint, Vector2(nearPoint.x - startPoint.x, _gridSize)));
            }

            if (farPoint.x > startX)
            {
                startX = farPoint.x;
            }
            if (bottomFarValues[nearVal] > 1) break;
        }

        if (startX < bottomLeft.x + room1Rect.size.x)
        {
            Vector2 startPoint(startX, bottomLeft.y - _gridSize - 1);
            Vector2 endPoint(bottomLeft.x + room1Rect.size.x, bottomLeft.y);
            _walls.push_back(Rect(startPoint, Vector2(endPoint.x - startPoint.x, _gridSize)));
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
                _walls.push_back(Rect(startPoint, Vector2(_gridSize, nearPoint.y - startPoint.y)));
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
            _walls.push_back(Rect(startPoint, Vector2(_gridSize, endPoint.y - startPoint.y)));
        }

        // right border
        startY = rightTop.y;
        for (auto const& nearVal : rightOrderedList)
        {
            Vector2 nearPoint(rightTop + (down * nearVal));
            Vector2 farPoint(rightTop + (down * rightFarValues[nearVal]));
            if (startY < nearPoint.y && nearVal >= 0)
            {
                Vector2 startPoint(rightTop.x - _gridSize - 1, startY);
                _walls.push_back(Rect(startPoint, Vector2(_gridSize, nearPoint.y - startPoint.y)));
            }

            if (farPoint.y > startY)
            {
                startY = farPoint.y;
            }
            if (rightFarValues[nearVal] > 1) break;
        }

        if (startY < rightTop.y + room1Rect.size.y)
        {
            Vector2 startPoint(rightTop.x - _gridSize - 1, startY);
            Vector2 endPoint(rightTop.x, rightTop.y + room1Rect.size.y);
            _walls.push_back(Rect(startPoint, Vector2(_gridSize, endPoint.y - startPoint.y)));
        }
    }
}
