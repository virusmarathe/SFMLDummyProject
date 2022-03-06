#include "NavMesh.h"
#include "Framework/Primitives.h"
#include <iostream>

#define ROOT_TWO 1.41421356237f

void NavMesh::generateNodes(std::vector<Rect> terrain, float gridSize)
{
    _nodeMap.clear();
    _gridSize = gridSize;

    for (auto& room : terrain)
    {
        for (float x = room.pos.x; x < room.pos.x + room.size.x; x += gridSize)
        {
            for (float y = room.pos.y; y < room.pos.y + room.size.y; y += gridSize)
            {
                // add node to graph
                _nodeMap[Vector2(x, y)] = NavNode(Vector2(x,y));
            }
        }
    }
    generateAdjacencyLists();
}

void NavMesh::generateAdjacencyLists()
{
    // generate adjacency list
    for (auto& pair : _nodeMap)
    {
        if (pair.second.layer == FREE)
        {
            addAdjacentNode(pair.second, Vector2::right(), _gridSize);
            addAdjacentNode(pair.second, Vector2::left(), _gridSize);
            addAdjacentNode(pair.second, Vector2::up(), _gridSize);
            addAdjacentNode(pair.second, Vector2::down(), _gridSize);
            addAdjacentNode(pair.second, Vector2::upRight(), _gridSize * ROOT_TWO);
            addAdjacentNode(pair.second, Vector2::upLeft(), _gridSize * ROOT_TWO);
            addAdjacentNode(pair.second, Vector2::downRight(), _gridSize * ROOT_TWO);
            addAdjacentNode(pair.second, Vector2::downLeft(), _gridSize * ROOT_TWO);
        }        
    }
}

void NavMesh::registerObstacle(Rect rect)
{
    for (float x = rect.pos.x; x < rect.pos.x + rect.size.x; x += _gridSize)
    {
        for (float y = rect.pos.y; y < rect.pos.y + rect.size.y; y += _gridSize)
        {
            // node[x][y] becomes an obstacle?
            _nodeMap[Vector2(x, y)].layer = NavLayer::OBSTACLE;
        }
    }
}

void NavMesh::debugShow()
{
    for (auto& pair : _nodeMap)
    {
        if (pair.second.layer == OBSTACLE)
        {
            Primitives::DrawRectShape(Rect(pair.second.coord, Vector2(_gridSize - 20, _gridSize - 20)), sf::Color::Green);
        }
        else
        {
            Primitives::DrawRectShape(Rect(pair.second.coord, Vector2(_gridSize - 20, _gridSize - 20)), sf::Color::Red);
        }
    }
}

void NavMesh::addAdjacentNode(NavNode& node, Vector2 direction, float cost)
{
    if (_nodeMap.find(node.coord + (direction * _gridSize)) != _nodeMap.end())
    {
        node.adjacencyList.push_back(NodeConnection(&(_nodeMap[node.coord + (direction * _gridSize)]), cost));
    }
}
