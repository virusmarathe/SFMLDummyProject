#pragma once

#include "Entity.h"
#include "EntityMemoryPool.h"
#include <vector>
#include <map>

typedef std::vector<Entity> EntityList;
typedef std::map<std::string, EntityList> EntityMap;
typedef std::map<size_t, Entity> EntityIDMap;

class EntityManager
{

public:
	EntityManager();

	Entity addEntity(const std::string& tag);

	EntityList getEntities();
	EntityList getEntities(std::string tag);

	void update();

	void destroyAll();
	
	Entity operator[](size_t id) { return _entityList[id]; }


private:
	EntityList _entityList;
	size_t _totalEntities = 0;
};

