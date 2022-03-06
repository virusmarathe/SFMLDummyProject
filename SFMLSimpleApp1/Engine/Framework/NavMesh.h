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
	
	NavNode(Vector2 coord) : coord(coord) {}
	NavNode() {}
};

class NavMesh
{
public:
	void generateNodes(std::vector<Rect> terrain, float gridSize);
	void generateAdjacencyLists();
	void registerObstacle(Rect rect);
	void debugShow();

private:
	void addAdjacentNode(NavNode& node, Vector2 direction, float cost);

private:
	float _gridSize = 50;
	std::unordered_map<Vector2, NavNode, Vector2HashFunction> _nodeMap;
};

