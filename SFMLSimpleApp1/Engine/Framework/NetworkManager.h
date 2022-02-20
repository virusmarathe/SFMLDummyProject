#pragma once
#include "SFML/Network.hpp"
#include <map>
#include <vector>

class GameEngine;

const double NETWORK_SEND_RATE = 0.03;

class NetworkManager
{
public:

	struct Connection
	{
		sf::IpAddress address;
		unsigned short port;

		Connection(sf::IpAddress ip, unsigned short port) : address(ip), port(port) {}
	};

	void init(GameEngine* engineRef);
	void host();
	void connect();
	void sendToServer(sf::Packet packet);
	void sendToClient(sf::Packet packet, Connection& connection);
	void sendToAllClients(sf::Packet& packet);
	void addToUpdatePacket(sf::Packet& packet);
	void serverDestroyEntity(size_t entID);
	void receive();
	void update(float dt);

	static bool isClient;
	static bool isServer;

	enum PacketType
	{
		CONNECT_REQUEST,
		CONNECT_CONFIRM,
		SCENE_EVENT,
		TRANSFORM,
		DESTROY_ENT
	};	

private:
	unsigned short bindSocket();

	sf::UdpSocket _serverSocket;
	std::vector<Connection> _clients;
	GameEngine* _engineRef;
	float _networkTimer = 0;
	sf::Packet _updatePacket;
};

