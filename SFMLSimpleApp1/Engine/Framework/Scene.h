#pragma once

#include "Entity/EntityManager.h"
#include "SFML/Network.hpp"

class GameEngine;
struct Action;

class Scene
{
public:
	virtual void init() = 0;
	virtual void preUpdate(float dt) {}
	virtual void postUpdate(float dt) {}
	virtual void sDoAction(const Action& action) = 0;
	virtual void onClientReady(int clientID) {} // called on client
	virtual void onClientConnectedToServer(int clientID) { } // called on server
	virtual void handlePacket(sf::Packet& packet) { }
	void updateEntityList() { _entities.update(); }

	void setEngineRefs(GameEngine* engine, sf::RenderWindow* window);

	EntityManager* getEntityManager() { return &_entities; }

protected:
	GameEngine* _engine = nullptr;
	sf::RenderWindow* _window = nullptr;
	EntityManager _entities;
	int _clientID = -1;
};

