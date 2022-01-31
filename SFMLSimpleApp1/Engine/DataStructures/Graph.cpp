#include "Graph.h"
#include <iostream>
#include <queue>

void Graph::addNode(size_t ID)
{
	if (_nodesMap.find(ID) == _nodesMap.end())
	{
		_nodesMap[ID] = GraphNode(ID);
	}
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

Graph Graph::getMinSpanTree()
{
	Graph minGraph;

	float minEdgeCost = INT_MAX;
	size_t startNodeID = 0;
	size_t endNodeID = 0;

	for (auto const& node : _nodesMap)
	{
		for (auto const& edgeNode : node.second.adjacencyList)
		{
			if (edgeNode.cost < minEdgeCost)
			{
				minEdgeCost = edgeNode.cost;
				startNodeID = node.first;
				endNodeID = edgeNode.node->ID;
			}
		}
	}

	std::unordered_set<size_t> idsVisited;
	idsVisited.insert(startNodeID);
	idsVisited.insert(endNodeID);

	minGraph.addNode(startNodeID);
	minGraph.addNode(endNodeID);
	minGraph.addEdge(startNodeID, endNodeID, minEdgeCost);

	while (idsVisited.size() < _nodesMap.size())
	{
		minEdgeCost = INT_MAX;
		for (auto const& startID : idsVisited)
		{
			GraphNode node = _nodesMap[startID];
			for (auto const& edgeNode : node.adjacencyList)
			{
				if (idsVisited.find(edgeNode.node->ID) == idsVisited.end())
				{
					if (edgeNode.cost < minEdgeCost)
					{
						minEdgeCost = edgeNode.cost;
						startNodeID = startID;
						endNodeID = edgeNode.node->ID;
					}
				}
			}
		}

		idsVisited.insert(startNodeID);
		idsVisited.insert(endNodeID);
		minGraph.addNode(startNodeID);
		minGraph.addNode(endNodeID);
		minGraph.addEdge(startNodeID, endNodeID, minEdgeCost);
	}

	return minGraph;
}
