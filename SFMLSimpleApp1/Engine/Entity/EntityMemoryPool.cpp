#include "EntityMemoryPool.h"
#include <iostream>

const std::string& EntityMemoryPool::getTag(size_t entityID)
{
	return _tags[entityID];
}

void EntityMemoryPool::setTag(size_t entityID, std::string tag)
{
	_tags[entityID] = tag;
}

bool EntityMemoryPool::isActive(size_t entityID)
{
	return _active[entityID];
}

const size_t index = 0;

void EntityMemoryPool::destroy(size_t entityID)
{
	if (entityID > MAX_ENTITIES) return;

	_active[entityID] = false;
	_tags[entityID] = "";
	removeComponent<CAnimation>(entityID);
	removeComponent<CCollisionEvent>(entityID);
	removeComponent<CDamage>(entityID);
	removeComponent<CHealth>(entityID);
	removeComponent<CInput>(entityID);
	removeComponent<CNetID>(entityID);
	removeComponent<CNetworkTransform>(entityID);
	removeComponent<CPhysicsAnimator>(entityID);
	removeComponent<CPhysicsBody>(entityID);
	removeComponent<CQuad>(entityID);
	removeComponent<CRectCollider>(entityID);
	removeComponent<CShapeLine>(entityID);
	removeComponent<CShapeRect>(entityID);
	removeComponent<CSprite>(entityID);
	removeComponent<CText>(entityID);
	removeComponent<CTransform>(entityID);
}

size_t EntityMemoryPool::getNextAvailableID()
{
	for (size_t i = 0; i < _active.size(); i++)
	{
		if (!_active[i]) return i;
	}

	return 0;
}

size_t EntityMemoryPool::addEntity(std::string tag)
{
	size_t index = getNextAvailableID();
	_tags[index] = tag;
	_active[index] = true;
	return index;
}

void EntityMemoryPool::resetPool()
{
	for (size_t i = 0; i < _active.size(); i++)
	{
		destroy(i);
	}
}

EntityMemoryPool::EntityMemoryPool(size_t maxEntities) : _numEntities(maxEntities)
{
	_pool = {
		std::vector<CAnimation>(maxEntities, CAnimation("")),
		std::vector<CCollisionEvent>(maxEntities, CCollisionEvent()),
		std::vector<CDamage>(maxEntities, CDamage()),
		std::vector<CHealth>(maxEntities, CHealth()),
		std::vector<CInput>(maxEntities, CInput()),
		std::vector<CNetID>(maxEntities, CNetID()),
		std::vector<CNetworkTransform>(maxEntities, CNetworkTransform()),
		std::vector<CPhysicsAnimator>(maxEntities, CPhysicsAnimator()),
		std::vector<CPhysicsBody>(maxEntities, CPhysicsBody()),
		std::vector<CQuad>(maxEntities, CQuad()),
		std::vector<CRectCollider>(maxEntities, CRectCollider()),
		std::vector<CShapeLine>(maxEntities, CShapeLine()),
		std::vector<CShapeRect>(maxEntities, CShapeRect()),
		std::vector<CSprite>(maxEntities, CSprite()),
		std::vector<CText>(maxEntities, CText()),
		std::vector<CTransform>(maxEntities, CTransform())
	};

	_tags = std::vector<std::string>(maxEntities, "");
	_active = std::vector<bool>(maxEntities, false);
}
