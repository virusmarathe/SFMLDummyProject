#pragma once
#include "Framework/Scene.h"
#include <queue>

class Assets;

struct RoomCompare 
{
	bool operator()(Entity e1, Entity e2)
	{
		return e2.getComponent<CRectCollider>().rect > e1.getComponent<CRectCollider>().rect;
	}
};

class Scene_DungeonTest : public Scene
{
public:

	// Inherited via Scene
	virtual void init() override;

	virtual void preUpdate(float dt) override;
	virtual void postUpdate(float dt) override;

	virtual void sDoAction(const Action& action) override;


private:
	
	void generateRooms(int numRooms);
	Entity createRoom();
	void createWall(Rect rect, std::string assetName);
	Entity spawnPlayer();
	void fireBullet(Vector2 mouseLocation);
	void sInput();
	void sHandleCollision();

	std::shared_ptr<Assets> _assets;
	Entity _camera;
	Entity _player1Entity;

	bool _separatedRooms = true;
	bool _createRoomGraph = false;
	bool _createBorder = false;
	bool _freeCam = false;
	bool _firing = false;
	float _fireCooldownTimer = 0;
	Vector2 _firePos;

	EntityList _rooms;
};

