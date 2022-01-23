#pragma once
#include "Framework/Scene.h"

class Assets;

class Scene_AnimationTest : public Scene
{
public:
	// Inherited via Scene
	virtual void init() override;
	virtual void update(float dt) override;
	virtual void sDoAction(const Action& action) override;

private:
	std::shared_ptr<Entity> spawnPlayer();
	void sInput();

	std::shared_ptr<Assets> _assets;
	std::shared_ptr<Entity> _player1Entity;
	std::shared_ptr<Entity> _camera;
};

