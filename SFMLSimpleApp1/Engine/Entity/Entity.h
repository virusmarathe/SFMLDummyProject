#pragma once

#include "Components/CTransform.h"
#include "Components/CSprite.h"
#include "Components/CRectCollider.h"
#include "Components/CText.h"
#include "Components/CPhysicsBody.h"
#include "Components/CInput.h"
#include "Components/CPlayerController.h"

class Entity
{

public:
	std::shared_ptr<CTransform> transform;
	std::shared_ptr<CSprite> sprite;
	std::shared_ptr<CRectCollider> collider;
	std::shared_ptr<CText> text;
	std::shared_ptr<CPhysicsBody> physics;
	std::shared_ptr<CInput> input;
	std::shared_ptr<CPlayerController> controller;

	bool operator==(const Entity& rhs) { return _id == rhs._id; }
	bool operator!=(const Entity& rhs) { return _id != rhs._id; }
	std::string tag() { return _tag; }
	void destroy() { _valid = false; }
	bool isValid() { return _valid; }

private:

	Entity(const size_t id, const std::string& tag) : _id(id), _tag(tag) { }

	const size_t _id = 0;
	const std::string _tag = "Default";
	bool _valid = true;

	friend class EntityManager;
};