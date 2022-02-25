#include "EntityManager.h"
#include "EntityMemoryPool.h"

static EntityList emptyList;

EntityManager::EntityManager()
{
	for (size_t i = 0; i < MAX_ENTITIES; i++)
	{
		_entityList.push_back(Entity(i));
	}
}

Entity EntityManager::addEntity(const std::string& tag)
{
	size_t index = EntityMemoryPool::Instance().addEntity(tag);
	return Entity(index);
}

EntityList EntityManager::getEntities()
{
	EntityList list;
	for (Entity ent : _entityList)
	{
		if (ent.isValid()) list.push_back(ent);
	}
	return list;
}

EntityList EntityManager::getEntities(std::string tag)
{
	EntityList list;
	for (Entity ent : _entityList)
	{
		if (ent.isValid() && ent.tag() == tag) list.push_back(ent);
	}
	return list;
}

void EntityManager::update()
{
}

void EntityManager::destroyAll()
{
	EntityMemoryPool::Instance().resetPool();
}
