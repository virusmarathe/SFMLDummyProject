#pragma once

#include "Component.h"
#include <string>

class CAnimation : public Component
{
public:
	CAnimation() {}
	CAnimation(std::string name) : name(name), currentFrame(-1), frameTimer(0), duration(1), numFrames(0), frameWidth(0), frameHeight(0) { }

	std::string name = "";
	int currentFrame = -1;
	float frameTimer = 0;
	float duration = 1;
	int numFrames = 0;
	int frameWidth = 0;
	int frameHeight = 0;
	bool flipX = false;
};