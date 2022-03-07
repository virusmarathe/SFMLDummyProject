#include "NavMesh.h"
#include "Framework/Primitives.h"
#include <iostream>
#include <queue>
#include <algorithm>

#define ROOT_TWO 1.41421356237f

struct NavNodeCompare
{
    bool operator()(const NavNode* lhs, const NavNode* rhs)
    {
        return lhs->fCost > rhs->fCost;
    }
};

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
            Primitives::DrawRectShape(Rect(pair.second.coord, Vector2(_gridSize - 20, _gridSize - 20)), sf::Color::Red);
        }
        else
        {
            Primitives::DrawRectShape(Rect(pair.second.coord, Vector2(_gridSize - 20, _gridSize - 20)), sf::Color::Green);
        }
    }
}

std::vector<Vector2> NavMesh::getPath(NavNode* startNode, NavNode* endNode)
{
    std::vector<Vector2> path;
    setHCosts(endNode);
    startNode->gCost = 0;

    std::priority_queue<NavNode*, std::vector<NavNode*>, NavNodeCompare> nodesToVisit;
    nodesToVisit.push(startNode);
    bool foundEnd = false;

    while (!nodesToVisit.empty())
    {
        NavNode* curNode = nodesToVisit.top();

        if (curNode == endNode)
        {
            foundEnd = true;
            break;
        }

        nodesToVisit.pop();
        if (curNode->processed) continue;

        for (auto& neighbor : curNode->adjacencyList)
        {
            if (neighbor.node->layer == FREE && !neighbor.node->processed)
            {
                if (neighbor.node->gCost > curNode->gCost + neighbor.cost)
                {
                    neighbor.node->gCost = curNode->gCost + neighbor.cost;
                    neighbor.node->parent = curNode;
                }
                neighbor.node->fCost = neighbor.node->hCost + neighbor.node->gCost;
                nodesToVisit.push(neighbor.node);
            }
        }

        curNode->processed = true;
    }

    if (foundEnd)
    {
        NavNode* curNode = endNode;
        while (curNode != nullptr)
        {
            path.push_back(curNode->coord);
            curNode = curNode->parent;
        }

        std::reverse(path.begin(), path.end());
    }

    return path;
}

NavNode* NavMesh::getNodeAtCoord(Vector2 coord)
{
    Vector2 gridCoord((int)coord.x - ((int)coord.x % (int)_gridSize), (int)coord.y - ((int)coord.y % (int)_gridSize));

    if (_nodeMap.find(gridCoord) != _nodeMap.end())
    {
        return &(_nodeMap[gridCoord]);
    }

    return nullptr;
}

void NavMesh::addAdjacentNode(NavNode& node, Vector2 direction, float cost)
{
    if (_nodeMap.find(node.coord + (direction * _gridSize)) != _nodeMap.end())
    {
        node.adjacencyList.push_back(NodeConnection(&(_nodeMap[node.coord + (direction * _gridSize)]), cost));
    }
}

void NavMesh::setHCosts(NavNode* endNode)
{
    for (auto& pair : _nodeMap)
    {
        if (pair.second.layer != OBSTACLE)
        {
            pair.second.hCost = Vector2::dist(endNode->coord, pair.second.coord);
        }
        pair.second.processed = false;
        pair.second.parent = nullptr;
        pair.second.gCost = (float)INT_MAX;
    }
}
