#include "Graph.h"
#include <iostream>

void Graph::addNode(size_t ID)
{
	_nodesMap[ID] = GraphNode(ID);
}

void Graph::addEdge(size_t startID, size_t endID, float cost)
{
	if (_nodesMap[startID].IDList.find(endID) == _nodesMap[startID].IDList.end())
	{
		_nodesMap[startID].IDList.insert(endID);
		_nodesMap[startID].adjacencyList.push_back(GraphEdge(&_nodesMap[endID], cost));
	}
	if (_nodesMap[endID].IDList.find(startID) == _nodesMap[endID].IDList.end())
	{
		_nodesMap[endID].IDList.insert(startID);
		_nodesMap[endID].adjacencyList.push_back(GraphEdge(&_nodesMap[startID], cost));
	}
}

void Graph::printGraph()
{
	for (auto const& node : _nodesMap)
	{
		std::cout << node.second.ID << ":   ";
		for (auto const& edgeNode : node.second.adjacencyList)
		{
			std::cout << edgeNode.node->ID << "[" << edgeNode.cost << "]" << " ";
		}
		std::cout << std::endl;
	}
}
