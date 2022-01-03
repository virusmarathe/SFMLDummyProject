#pragma once

#include "Entity/EntityManager.h"

class GameEngine;
struct Action;

class Scene
{
public:
	virtual void init() = 0;
	virtual void update(float dt) = 0;
	virtual void sDoAction(const Action& action) = 0;
	virtual void sRender() = 0;

	void setEngineRefs(GameEngine* engine, sf::RenderWindow* window) { _engine = engine; _window = window; }

protected:
	GameEngine* _engine = nullptr;
	sf::RenderWindow* _window = nullptr;
	EntityManager _entities;
};

