#pragma once
#include "Framework/Scene.h"
#include "Resources/Assets.h"

class Scene_Menu : public Scene
{
public:

	virtual void init() override;

	virtual void sDoAction(const Action& action) override;

	virtual void onClientReady(int clientID) override;

private:

	std::shared_ptr<Assets> _assets;
};

