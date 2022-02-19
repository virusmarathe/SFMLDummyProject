#pragma once

#include "Entity/EntityManager.h"
#include "SFML/Network.hpp"

class GameEngine;
struct Action;

class Scene
{
public:
	virtual void init() = 0;
	virtual void update(float dt) = 0;
	virtual void sDoAction(const Action& action) = 0;
	virtual void onClientConnected() {}
	virtual void handlePacket(sf::Packet& packet) { }
	void updateEntityList() { _entities.update(); }

	void setEngineRefs(GameEngine* engine, sf::RenderWindow* window);

	std::shared_ptr<Entity> getEntity(size_t id) { return _entities[id]; }

protected:
	GameEngine* _engine = nullptr;
	sf::RenderWindow* _window = nullptr;
	EntityManager _entities;
};

