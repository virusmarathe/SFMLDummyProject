#pragma once
#include <tuple>
#include <vector>
#include "Components/CAnimation.h"
#include "Components/CCollisionEvent.h"
#include "Components/CDamage.h"
#include "Components/CHealth.h"
#include "Components/CInput.h"
#include "Components/CNetID.h"
#include "Components/CNetworkTransform.h"
#include "Components/CPhysicsAnimator.h"
#include "Components/CPhysicsBody.h"
#include "Components/CQuad.h"
#include "Components/CRectCollider.h"
#include "Components/CShapeLine.h"
#include "Components/CShapeRect.h"
#include "Components/CSprite.h"
#include "Components/CText.h"
#include "Components/CTransform.h"

typedef std::tuple<
	std::vector<CAnimation>,
	std::vector<CCollisionEvent>,
	std::vector<CDamage>,
	std::vector<CHealth>,
	std::vector<CInput>,
	std::vector<CNetID>,
	std::vector<CNetworkTransform>,
	std::vector<CPhysicsAnimator>,
	std::vector<CPhysicsBody>,
	std::vector<CQuad>,
	std::vector<CRectCollider>,
	std::vector<CShapeLine>,
	std::vector<CShapeRect>,
	std::vector<CSprite>,
	std::vector<CText>,
	std::vector<CTransform>
> EntityComponentVectorTuple;

const size_t MAX_ENTITIES = 20000;

class EntityMemoryPool
{
private:
	EntityMemoryPool(size_t maxEntities);

	size_t _numEntities;
	EntityComponentVectorTuple _pool;
	std::vector<std::string> _tags;
	std::vector<bool> _active;

public:
	static EntityMemoryPool& Instance() 
	{
		static EntityMemoryPool pool(MAX_ENTITIES);
		return pool;
	}

	template <typename T>
	T& getComponent(size_t entityID);

	template <typename T>
	bool hasComponent(size_t entityID);

	template <typename T>
	void removeComponent(size_t entityID);

	template <typename T>
	T& addComponent(size_t entityID);

	const std::string& getTag(size_t entityID);
	void setTag(size_t entityID, std::string tag);

	bool isActive(size_t entityID);

	void destroy(size_t entityID);

	size_t getNextAvailableID();

	size_t addEntity(std::string tag);

	void resetPool();

};

template<typename T>
inline T& EntityMemoryPool::getComponent(size_t entityID)
{
	return std::get<std::vector<T>>(_pool)[entityID];
}

template<typename T>
inline bool EntityMemoryPool::hasComponent(size_t entityID)
{
	return std::get<std::vector<T>>(_pool)[entityID].enabled;
}

template<typename T>
inline void EntityMemoryPool::removeComponent(size_t entityID)
{
	std::get<std::vector<T>>(_pool)[entityID].enabled = false;
	std::get<std::vector<T>>(_pool)[entityID] = {};
}

template<typename T>
inline T& EntityMemoryPool::addComponent(size_t entityID)
{
	std::get<std::vector<T>>(_pool)[entityID].enabled = true;
	return std::get<std::vector<T>>(_pool)[entityID];
}
