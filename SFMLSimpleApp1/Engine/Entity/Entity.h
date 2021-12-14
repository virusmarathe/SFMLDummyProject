#pragma once

#include "Components/CTransform.h"
#include "Components/CSprite.h"
#include "Components/CRectCollider.h"
#include "Components/CText.h"

class Entity
{
	const size_t _id = 0;

public:
	std::shared_ptr<CTransform> transform;
	std::shared_ptr<CSprite> sprite;
	std::shared_ptr<CRectCollider> collider;
	std::shared_ptr<CText> text;

	Entity(size_t id) : _id(id) { }
	bool operator==(const Entity& rhs) { return _id == rhs._id; }
	bool operator!=(const Entity& rhs) { return _id != rhs._id; }
};