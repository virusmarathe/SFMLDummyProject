#pragma once

#include "Component.h"
#include <string>

class CAnimation : public Component
{
public:
	CAnimation(std::string name) : name(name), currentFrame(-1), frameTimer(0), duration(1), numFrames(0), frameWidth(0), frameHeight(0) { }

	std::string name;
	int currentFrame;
	float frameTimer;
	float duration;
	int numFrames;
	int frameWidth;
	int frameHeight;
};