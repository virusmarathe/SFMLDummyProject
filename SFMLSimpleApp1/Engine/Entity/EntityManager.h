#pragma once

#include "Entity.h"
#include <vector>
#include <map>

typedef std::vector<std::shared_ptr<Entity>> EntityList;
typedef std::map<std::string, EntityList> EntityMap;
typedef std::map<size_t, std::shared_ptr<Entity>> EntityIDMap;

class EntityManager
{

public:
	EntityManager();

	std::shared_ptr<Entity> addEntity(const std::string& tag);
	std::shared_ptr<Entity> addEntity(size_t id, const std::string& tag);

	EntityList& getEntities() { return _entityList; }
	EntityList& getEntities(const std::string& tag);

	void update();

	void destroyAll();

	std::shared_ptr<Entity> operator[](size_t id) { return _entityIDMap[id]; }

private:
	EntityList _entityList, _entitiesToAdd, _entitiesToRemove;
	EntityMap _entityMap;
	EntityIDMap _entityIDMap;
	size_t _totalEntities = 0;
};

