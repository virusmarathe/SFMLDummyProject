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
				if (ent.hasComponent<CNetworkTransform>() && ent.hasComponent<CNetID>() && ent.isValid())
				{
					auto& netTransform = ent.getComponent<CNetworkTransform>();
					netTransform.position = ent.getComponent<CTransform>().position;
					sf::Packet pack;
					pack << NetworkManager::PacketType::TRANSFORM << ent.getComponent<CNetID>().netID << netTransform.position;
					if (ent.hasComponent<CPhysicsBody>())
					{
						netTransform.velocity = ent.getComponent<CPhysicsBody>().velocity;
						pack << netTransform.velocity;
					}
					else pack << Vector2();
					_engine->updatePacket(pack);
				}
			}
		}
		else
		{
			for (auto ent : _entities->getEntities())
			{
				if (ent.hasComponent<CNetworkTransform>() && ent.isValid())
				{
					if (ent.hasComponent<CPhysicsBody>())
					{
						Vector2 pos = ent.getComponent<CTransform>().position;
						Vector2 desiredPos = ent.getComponent<CNetworkTransform>().position;
						float distSqr = (desiredPos - pos).magnitudeSqr();
						if (distSqr < 500)
						{
							ent.getComponent<CPhysicsBody>().velocity = ent.getComponent<CNetworkTransform>().velocity;
						}
						else
						{
							ent.getComponent<CTransform>().position = ent.getComponent<CNetworkTransform>().position;
						}
					}
					else
					{
						ent.getComponent<CTransform>().position = ent.getComponent<CNetworkTransform>().position;
					}
				}
			}
		}
	}

private:
	GameEngine* _engine;
};
