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
	virtual void sRender() override;

private:
	std::shared_ptr<Entity> spawnPlayer();
	void sAnimation(float dt);
	void sInput();
	void sMovement(float dt);

	std::shared_ptr<Assets> _assets;
	std::shared_ptr<Entity> _player1Entity;
};

