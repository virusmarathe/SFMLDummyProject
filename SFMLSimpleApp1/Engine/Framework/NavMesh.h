#pragma once
#include <vector>
#include "Math/Rect.h"
#include "Math/Vector2.h"
#include <unordered_map>

enum NavLayer
{
	FREE,
	OBSTACLE
};

struct NavNode;
struct NodeConnection
{
	NavNode* node = nullptr;
	float cost = (float)INT_MAX;

	NodeConnection(NavNode* node, float cost) : node(node), cost(cost) {}
};

struct NavNode
{
	Vector2 coord;
	NavLayer layer = FREE;
	std::vector<NodeConnection> adjacencyList;
	float hCost = (float)INT_MAX;
	float gCost = (float)INT_MAX;
	float fCost = (float)INT_MAX;
	bool processed = false;
	NavNode* parent = nullptr;

	NavNode(Vector2 coord) : coord(coord) {}
	NavNode() {}
	bool operator<(const NavNode& rhs) const { return fCost < rhs.fCost; }
};

class NavMesh
{
public:
	void generateNodes(std::vector<Rect> terrain, float gridSize);
	void generateAdjacencyLists();
	void registerObstacle(Rect rect);
	void notifyObstacleDestroyed(Rect rect);
	void debugShow();
	std::vector<Vector2> getPath(NavNode* startNode, NavNode* endNode);
	NavNode* getNodeAtCoord(Vector2 coord);

private:
	void addAdjacentNode(NavNode& node, Vector2 direction, float cost);
	void setHCosts(NavNode* endNode);

private:
	float _gridSize = 50;
	std::unordered_map<Vector2, NavNode, Vector2HashFunction> _nodeMap;
};

