#pragma once
#include "Component.h"

class CNetID : public Component
{
public:
	unsigned int netID;

	CNetID(unsigned int id) : netID(id) { }
};