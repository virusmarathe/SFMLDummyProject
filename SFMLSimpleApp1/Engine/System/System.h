#pragma once

#include "Entity/EntityManager.h"

enum Priority
{
	INPUT = 10,
	PHYSICS = 20,
	COLLISION = 30,
	UPDATE = 40,
	RENDER = 50
};

class System
{
public:
	System(EntityManager * entityManagerRef, int priority) : _entities(entityManagerRef), priority(priority) {}

	virtual void update(float dt) {}

	bool operator< (const System& rhs) const { return priority < rhs.priority; }
	bool operator> (const System& rhs) const { return priority > rhs.priority; }

protected:
	EntityManager* _entities;
	int priority;
};

struct SystemCompare
{
	bool operator()(const System& lhs, const System& rhs)
	{
		return (lhs < rhs);
	}
};
