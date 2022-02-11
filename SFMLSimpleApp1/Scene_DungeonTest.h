#pragma once
#include "Framework/Scene.h"
#include <queue>

class Assets;

struct RoomCompare 
{
	bool operator()(const std::shared_ptr<Entity> e1, const std::shared_ptr<Entity> e2)
	{
		return e2->getComponent<CRectCollider>()->rect > e1->getComponent<CRectCollider>()->rect;
	}
};

class Scene_DungeonTest : public Scene
{
public:

	// Inherited via Scene
	virtual void init() override;

	virtual void update(float dt) override;

	virtual void sDoAction(const Action& action) override;


private:
	
	void generateRooms(int numRooms);
	std::shared_ptr<Entity> createRoom();
	void createWall(Rect rect, std::string assetName);
	std::shared_ptr<Entity> spawnPlayer();
	void sInput();

	std::shared_ptr<Assets> _assets;
	std::shared_ptr<Entity> _camera;
	std::shared_ptr<Entity> _player1Entity;

	bool _separatedRooms = true;
	bool _createRoomGraph = false;
	bool _createBorder = false;
	bool _freeCam = false;

	EntityList _rooms;
};

