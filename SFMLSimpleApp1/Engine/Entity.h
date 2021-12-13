#pragma once

#include "Components/CTransform.h"
#include "Components/CSpriteRenderer.h"
#include "Components/CRectCollider.h"

class Entity
{
	int _id;

public:
	std::shared_ptr<CTransform> transform;
	std::shared_ptr<CSpriteRenderer> renderer;
	std::shared_ptr<CRectCollider> collider;

	Entity(int id) : _id(id) { }
	bool operator==(const Entity& rhs) { return _id == rhs._id; }
	bool operator!=(const Entity& rhs) { return _id != rhs._id; }
};