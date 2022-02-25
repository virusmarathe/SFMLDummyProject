#pragma once
#include "SFML/Network.hpp"
#include <map>
#include <vector>
#include "Entity/EntityManager.h"

class GameEngine;

const double NETWORK_SEND_RATE = 0.03;

class NetworkManager
{
public:

	struct Connection
	{
		sf::IpAddress address;
		unsigned short port;
		int clientID;

		Connection(sf::IpAddress ip, unsigned short port, int clientid) : address(ip), port(port), clientID(clientid) {}
	};

	void init(GameEngine* engineRef);
	void host();
	void connect();
	void sendToServer(sf::Packet packet);
	void sendToClient(sf::Packet packet, Connection& connection);
	void sendToAllClients(sf::Packet& packet);
	void addToUpdatePacket(sf::Packet& packet);
	void addToLateConnectPacket(sf::Packet& packet);
	void serverDestroyEntity(unsigned int netID);
	Entity serverCreateEntity(EntityManager* entities, std::string tag);
	void clientAddNetID(unsigned int netID, Entity ent) { _netIDToEntityMap[netID] = ent; }
	void receive();
	void update(float dt);

	static bool isClient;
	static bool isServer;
	static unsigned int nextNetID;
	static int nextClientID;
	static int clientID;

	enum PacketType
	{
		CONNECT_REQUEST,
		CONNECT_CONFIRM,
		SCENE_EVENT,
		TRANSFORM,
		DESTROY_ENT,
		LATE_CONNECT_REQUEST,
		INPUT_EVENT
	};	

private:
	unsigned short bindSocket();

	sf::UdpSocket _serverSocket;
	std::vector<Connection> _clients;
	GameEngine* _engineRef;
	float _networkTimer = 0;
	sf::Packet _updatePacket;
	std::map<unsigned int, Entity> _netIDToEntityMap;
	bool _connected = false;
	sf::Packet _lateConnectPacket;
};

