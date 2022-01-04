#pragma once

#include "Entity.h"
#include <vector>
#include <map>

typedef std::vector<std::shared_ptr<Entity>> EntityList;
typedef std::map<std::string, EntityList> EntityMap;

class EntityManager
{

public:
	EntityManager();

	std::shared_ptr<Entity> addEntity(const std::string& tag);

	EntityList& getEntities() { return _entityList; }
	EntityList& getEntities(const std::string& tag);

	void update();

	void destroyAll();

private:
	EntityList _entityList, _entitiesToAdd, _entitiesToRemove;
	EntityMap _entityMap;
	size_t _totalEntities = 0;
};

