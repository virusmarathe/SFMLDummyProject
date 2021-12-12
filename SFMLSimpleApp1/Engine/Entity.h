#pragma once

#include "Components/CTransform.h"
#include "Components/CSpriteRenderer.h"
#include "Components/CRectCollider.h"

class Entity
{
public:
	std::shared_ptr<CTransform> transform;
	std::shared_ptr<CSpriteRenderer> renderer;
	std::shared_ptr<CRectCollider> collider;
};