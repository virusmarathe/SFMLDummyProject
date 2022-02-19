#pragma once
#include "SFML/Network.hpp"
#include <map>
#include <vector>

class GameEngine;

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
	void receive();

	static bool isClient;
	static bool isServer;

	enum PacketType
	{
		CONNECT_REQUEST,
		CONNECT_CONFIRM,
		SCENE_EVENT,
		TRANSFORM
	};	

private:
	unsigned short bindSocket();

	sf::UdpSocket _serverSocket;
	std::vector<Connection> _clients;
	GameEngine* _engineRef;
};

