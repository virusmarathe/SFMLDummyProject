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

	std::shared_ptr<Assets> _assets;
	std::shared_ptr<Entity> _camera;

	bool _separatedRooms = true;
	bool _createRoomGraph = false;

	EntityList _rooms;
};

