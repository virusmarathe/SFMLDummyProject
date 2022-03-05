#pragma once
#include "Math/Rect.h"
#include <vector>
#include <queue>
#include "DataStructures/Graph.h"

class ProceduralDungeon
{
public:
	void generateDungeon(int numRooms, float gridSize);
	std::vector<Rect>& getWalls() { return _walls; }
	std::vector<Rect>& getRooms() { return _finalRooms; }

private:
	Rect createRoomRect();
	void separateRooms();
	void createRoomGraph();
	void createHallways();
	void addSmallRooms();
	void createWallRects();

private:
	 std::vector<Rect> _allRooms;
	 float _gridSize, _hallwayWidthHalf;
	 std::vector<Rect> _finalRooms;
	 std::vector<Vector2> _hallways;
	 std::vector<Rect> _walls;
	 Graph _minGraph;
};

