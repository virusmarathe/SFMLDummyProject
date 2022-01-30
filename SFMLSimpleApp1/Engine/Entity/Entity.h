#pragma once

#include "Components/CTransform.h"
#include "Components/CSprite.h"
#include "Components/CRectCollider.h"
#include "Components/CText.h"
#include "Components/CPhysicsBody.h"
#include "Components/CInput.h"
#include "Components/CAnimation.h"
#include "Components/CPhysicsAnimator.h"
#include "Components/CCollisionEvent.h"
#include "Components/CShapeRect.h"
#include "Components/CShapeLine.h"
#include <tuple>

typedef std::tuple<
	std::shared_ptr<CTransform>,
	std::shared_ptr<CPhysicsBody>,
	std::shared_ptr<CSprite>,
	std::shared_ptr<CInput>,
	std::shared_ptr<CRectCollider>,
	std::shared_ptr<CText>,
	std::shared_ptr<CAnimation>,
	std::shared_ptr<CPhysicsAnimator>,
	std::shared_ptr<CCollisionEvent>,
	std::shared_ptr<CShapeRect>,
	std::shared_ptr<CShapeLine>
> ComponentTuple;

class Entity
{

public:

	bool operator==(const Entity& rhs) const { return _id == rhs._id; }
	bool operator!=(const Entity& rhs) const { return _id != rhs._id; }
	std::string tag() { return _tag; }
	void destroy() { _valid = false; }
	bool isValid() { return _valid; }

	template<typename T, typename... TArgs>
	std::shared_ptr<T> addComponent(TArgs&& ...args)
	{
		std::get<std::shared_ptr<T>>(_components) = std::make_shared<T>(std::forward<TArgs>(args)...);
		std::shared_ptr<T> component = getComponent<T>();
		component->has = true;
		return component;
	}

	template<typename T>
	std::shared_ptr<T> getComponent()
	{
		return std::get<std::shared_ptr<T>>(_components);
	}

	template<typename T>
	void removeComponent()
	{
		std::shared_ptr<T> component = getComponent<T>();
		if (component)
		{
			component->has = false;
		}
	}

	template<typename T>
	bool hasComponent()
	{
		if (getComponent<T>() == nullptr) return false;

		return getComponent<T>()->has;
	}

private:

	Entity(const size_t id, const std::string& tag) : _id(id), _tag(tag) { }

	const size_t _id = 0;
	const std::string _tag = "Default";
	bool _valid = true;
	ComponentTuple _components;

	friend class EntityManager;
};