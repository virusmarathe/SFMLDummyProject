#pragma once

#include <vector>
#include <map>
#include <unordered_set>

struct GraphNode;
struct GraphEdge
{
	GraphNode* node = nullptr;
	float cost;

	GraphEdge(GraphNode* node, float cost) : node(node), cost(cost) {}
};

struct GraphNode
{
	size_t ID;
	std::vector<GraphEdge> adjacencyList;
	std::unordered_set<size_t> IDList;

	GraphNode() : ID(0) { }
	GraphNode(size_t id) : ID(id) { }
};

class Graph
{
public:

	void addNode(size_t ID);
	void addEdge(size_t startID, size_t endID, float cost);

	void printGraph();

private:
	std::map<size_t, GraphNode> _nodesMap;
};