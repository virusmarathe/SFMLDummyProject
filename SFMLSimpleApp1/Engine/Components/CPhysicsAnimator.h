#pragma once
#include "Component.h"
#include <string>

class CPhysicsAnimator : public Component
{
public:
	CPhysicsAnimator(std::string idleAnim, std::string moveAnim) : idleAnimName(idleAnim), movingAnimName(moveAnim) { }

	std::string currentAnimName = "";
	std::string idleAnimName;
	std::string movingAnimName;
};