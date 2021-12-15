#include "EntityManager.h"

static EntityList emptyList;

EntityManager::EntityManager()
{
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto e = std::make_shared<Entity>(_totalEntities++, tag);
	_entitiesToAdd.push_back(e);
	return e;
}

EntityList& EntityManager::getEntities(const std::string& tag)
{
	if (_entityMap.find(tag) != _entityMap.end())
	{
		return _entityMap[tag];
	}

	return emptyList;
}

void EntityManager::update()
{
	// remove destroyed entities
	auto it = _entityList.begin();
	while (it != _entityList.end())
	{
		if (!(*it)->isValid())
		{
			auto tagIt = _entityMap[(*it)->tag()].begin();
			while (tagIt != _entityMap[(*it)->tag()].end())
			{
				if ((*tagIt) == (*it))
				{
					_entityMap[(*it)->tag()].erase(tagIt);
					break;
				}
				++tagIt;
			}
			it = _entityList.erase(it);
		}
		else
		{
			++it;
		}
	}

	// add new entities
	for (auto e : _entitiesToAdd)
	{
		_entityList.push_back(e);
		_entityMap[e->tag()].push_back(e);
	}
	_entitiesToAdd.clear();
}
