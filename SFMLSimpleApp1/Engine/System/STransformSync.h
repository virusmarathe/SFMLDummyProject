#pragma once
#include "System.h"
#include "Components/CNetworkTransform.h"
#include "Framework/NetworkManager.h"
#include "Framework/GameEngine.h"

class STransformSync : public System
{
public:
	STransformSync(GameEngine* engineRef, EntityManager* entityManagerRef, int priority) : _engine(engineRef), System(entityManagerRef, priority) { }

	virtual void update(float dt) override
	{
		if (NetworkManager::isServer)
		{
			for (auto ent : _entities->getEntities())
			{
				if (ent->hasComponent<CNetworkTransform>())
				{
					std::shared_ptr<CNetworkTransform> netTransform = ent->getComponent<CNetworkTransform>();
					netTransform->position = ent->getComponent<CTransform>()->position;
					if (netTransform->position != netTransform->lastPos)
					{
						sf::Packet pack;
						pack << NetworkManager::PacketType::TRANSFORM << ent->id() << netTransform->position;
						_engine->sendToAllClients(pack);
						netTransform->lastPos = netTransform->position;
					}
				}
			}
		}
		else
		{
			for (auto ent : _entities->getEntities())
			{
				if (ent->hasComponent<CNetworkTransform>())
				{
					ent->getComponent<CTransform>()->position = ent->getComponent<CNetworkTransform>()->position;
				}
			}
		}
	}

private:
	GameEngine* _engine;
};
