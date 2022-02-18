#include "NetworkManager.h"
#include "../../Game/Settings.h"
#include <iostream>
#include "Math/Vector2.h"

#define LOCAL_TEST 1

void NetworkManager::bindSocket()
{
    _socket.setBlocking(false);
    if (_socket.bind(Settings::SERVER_PORT) != sf::Socket::Done)
    {
        std::cout << "Error binding to port: " << Settings::SERVER_PORT << ". Trying local test port" << std::endl;
#if LOCAL_TEST
        _socket.bind(Settings::TEST_PORT);
#endif
    }
}

void NetworkManager::send(sf::Packet packet)
{
    if (_socket.send(packet, Settings::SERVER_IP, Settings::SERVER_PORT) != sf::Socket::Done) { std::cout << "Error sending packet!"; }
#if LOCAL_TEST
    if (_socket.send(packet, Settings::SERVER_IP, Settings::TEST_PORT) != sf::Socket::Done) { std::cout << "Error sending packet!"; }
#endif
}

void NetworkManager::receive()
{
    sf::IpAddress sender;
    unsigned short port;
    sf::Packet packet;
    sf::Socket::Status status = _socket.receive(packet, sender, port);
    if (status == sf::Socket::Done)
    {
        std::string sceneName;
        if (packet >> sceneName) std::cout << sceneName << std::endl;
        Vector2 val;
        if (packet >> val) std::cout << val.x << ", " << val.y << std::endl;
    }
}


