#pragma once
#include "SFML/Network.hpp"
#include <map>
#include <vector>

class NetworkManager
{
public:

	struct Connection
	{
		sf::IpAddress address;
		unsigned short port;

		Connection(sf::IpAddress ip, unsigned short port) : address(ip), port(port) {}
	};

	void host();
	void connect();
	void sendToServer(sf::Packet packet);
	void sendToClient(sf::Packet packet, Connection& connection);
	void receive();

	static bool isClient;
	static bool isServer;

	enum PacketType
	{
		CONNECT_REQUEST,
		CONNECT_CONFIRM
	};	

private:
	unsigned short bindSocket();

	sf::UdpSocket _serverSocket;
	std::vector<Connection> _clients;
};

