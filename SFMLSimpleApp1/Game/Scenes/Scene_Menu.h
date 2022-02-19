#pragma once
#include "Framework/Scene.h"
#include "Resources/Assets.h"

class Scene_Menu : public Scene
{
public:

	virtual void init() override;

	virtual void update(float dt) override;

	virtual void sDoAction(const Action& action) override;

private:

	std::shared_ptr<Assets> _assets;
};

