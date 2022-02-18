#pragma once
#include "SFML/Network.hpp"

class NetworkManager
{
public:
	NetworkManager() {}
	void bindSocket();
	void send(sf::Packet packet);
	void receive();

private:
	sf::UdpSocket _socket;
};

