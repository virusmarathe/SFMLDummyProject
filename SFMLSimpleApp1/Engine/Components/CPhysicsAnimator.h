#pragma once
#include "Component.h"
#include <string>

class CPhysicsAnimator : public Component
{
public:
	CPhysicsAnimator() {}
	CPhysicsAnimator(std::string idleAnim, std::string moveAnim, std::string fastMoveAnim, float velThreshold) : 
		idleAnimName(idleAnim),
		movingAnimName(moveAnim),
		fastMovingAnimName(fastMoveAnim),
		velThreshold(velThreshold) { }

	std::string currentAnimName = "";
	std::string idleAnimName = "";
	std::string movingAnimName = "";
	std::string fastMovingAnimName = "";
	float velThreshold = 0;
	bool isMovingRight = true;
};