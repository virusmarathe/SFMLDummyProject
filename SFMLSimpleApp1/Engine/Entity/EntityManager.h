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

	template <typename T>
	EntityList getEntitiesByType();

	void update();

	void destroyAll();
	
	Entity operator[](size_t id) { return _entityList[id]; }


private:
	EntityList _entityList;
	size_t _totalEntities = 0;
	size_t _maxEntityIndex = 0;
};

template<typename T>
inline EntityList EntityManager::getEntitiesByType()
{
	EntityList list;
	for (Entity ent : _entityList)
	{
		if (ent.isValid() && ent.hasComponent<T>()) list.push_back(ent);
		if (ent.id >= _maxEntityIndex) break;
	}
	return list;
}
