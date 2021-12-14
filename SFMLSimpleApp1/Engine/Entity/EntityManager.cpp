#include "EntityManager.h"

EntityManager::EntityManager()
{
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	std::shared_ptr<Entity> e = std::make_shared<Entity>(_totalEntities++);
	_entityList.push_back(e);
	_entityMap[tag].push_back(e);
	return e;
}

void EntityManager::removeEntity(const Entity& ent)
{
}
