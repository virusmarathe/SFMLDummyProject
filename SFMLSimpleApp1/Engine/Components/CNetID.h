#pragma once
#include "Component.h"

class CNetID : public Component
{
public:
	unsigned int netID;
	int ownerID = 0;

	CNetID(unsigned int id) : netID(id) { }
	CNetID(unsigned int id, int clientID) : netID(id), ownerID(clientID) { }
};