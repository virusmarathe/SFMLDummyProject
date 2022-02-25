#pragma once

#include "EntityMemoryPool.h"

class Entity
{

public:
	size_t id = MAX_ENTITIES+1; // init to value outside of scope

	bool operator==(const Entity& rhs) const { return id == rhs.id; }
	bool operator!=(const Entity& rhs) const { return id != rhs.id; }
	void operator=(const Entity& rhs) { id = rhs.id; }

	std::string tag() 
	{
		return EntityMemoryPool::Instance().getTag(id);
	}
	void destroy() 
	{
		EntityMemoryPool::Instance().destroy(id);
	}
	bool isValid() 
	{
		return EntityMemoryPool::Instance().isActive(id);
	}

	template<typename T, typename... TArgs>
	T& addComponent()
	{
		T& comp = EntityMemoryPool::Instance().addComponent<T>(id);
		return comp;
	}

	template<typename T>
	T& getComponent()
	{
		return EntityMemoryPool::Instance().getComponent<T>(id);
	}

	template<typename T>
	void removeComponent()
	{
		EntityMemoryPool::Instance().removeComponent<T>(id);
	}

	template<typename T>
	bool hasComponent()
	{
		return EntityMemoryPool::Instance().hasComponent<T>(id);
	}

	Entity() : id(MAX_ENTITIES + 1) { }

private:

	Entity(size_t id) : id(id) { }

	friend class EntityManager;
	friend class EntityMemoryPool;
};