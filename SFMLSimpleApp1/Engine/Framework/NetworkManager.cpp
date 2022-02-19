#include "NetworkManager.h"
#include "../../Game/Settings.h"
#include <iostream>
#include "Math/Vector2.h"

#define LOCAL_TEST 1

bool NetworkManager::isClient = false;
bool NetworkManager::isServer = false;

void NetworkManager::host()
{
    isServer = true;
    isClient = true;
    bindSocket();
}

void NetworkManager::connect()
{
    isClient = true;
    sf::Packet packet;
    packet << CONNECT_REQUEST << bindSocket();
    sendToServer(packet);
}

unsigned short NetworkManager::bindSocket()
{
    _serverSocket.setBlocking(false);
    if (isServer)
    {
        if (_serverSocket.bind(Settings::SERVER_PORT) != sf::Socket::Done)
        {
            std::cout << "Error binding to port: " << Settings::SERVER_PORT << std::endl;
        }
        return Settings::SERVER_PORT;
    }
    else
    {
        if (_serverSocket.bind(sf::Socket::AnyPort) != sf::Socket::Done)
        {
            std::cout << "Error binding to port: " << Settings::SERVER_PORT << std::endl;
        }
        std::cout << "bound to port " << _serverSocket.getLocalPort() << std::endl;
        return _serverSocket.getLocalPort();
    }
}

void NetworkManager::sendToServer(sf::Packet packet)
{
    if (_serverSocket.send(packet, Settings::SERVER_IP, Settings::SERVER_PORT) != sf::Socket::Done) { std::cout << "Error sending packet!"; }
}

void NetworkManager::sendToClient(sf::Packet packet, Connection& connection)
{
    if (_serverSocket.send(packet, connection.address, connection.port) != sf::Socket::Done) { std::cout << "Error sending packet!"; }
}

void NetworkManager::receive()
{
    sf::IpAddress sender;
    unsigned short port;
    sf::Packet packet;
    sf::Socket::Status status = _serverSocket.receive(packet, sender, port);
    if (status == sf::Socket::Done)
    {
        int pType;
        std::string sceneName;
        if (packet >> pType)
        {
            if (pType == CONNECT_REQUEST && isServer)
            {
                unsigned short clientPort;
                packet >> clientPort;
                std::cout << "Client " << sender.getPublicAddress() << " connected using port " << clientPort << std::endl;
                _clients.push_back(Connection(sender, clientPort));
                sf::Packet pack;
                pack << CONNECT_CONFIRM << sender.getPublicAddress().toString();
                sendToClient(pack, _clients[_clients.size()-1]);
            }
            else if (pType == CONNECT_CONFIRM && isClient)
            {
                std::cout << "Successfully connected to server!";
            }
        }
    }
}


