#pragma once
#include "Component.h"

class CNetID : public Component
{
public:
	unsigned int netID = 0;
	int ownerID = 0;

	CNetID() {}
	CNetID(unsigned int id) : netID(id) { }
	CNetID(unsigned int id, int clientID) : netID(id), ownerID(clientID) { }
};