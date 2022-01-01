#pragma once
#include "Framework/Scene.h"
#include "Resources/Assets.h"

class Scene_PhysicsTest : public Scene
{
public:

	virtual void init() override;

	virtual void update(float dt) override;

	virtual void sDoAction(const Action& action) override;

	virtual void sRender() override;

private:

	void sInput();
	void sPhysics(float dt);
	void sMovement(float dt);
	void sDebugDraw();

	void spawnNewBall();
	void spawnWall(Rect rect);
	void spawnPlayer(Vector2 pos, int playerNum);

	std::shared_ptr<Assets> _assets;
	bool _debugToggle = false;
	float _ballTimer = 0;
};

