#include "NetworkManager.h"
#include "../../Game/Settings.h"
#include <iostream>
#include "Math/Vector2.h"
#include "GameEngine.h"

#define LOCAL_TEST 1

bool NetworkManager::isClient = false;
bool NetworkManager::isServer = false;

void NetworkManager::init(GameEngine* engineRef)
{
    _engineRef = engineRef;
}

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

void NetworkManager::sendToAllClients(sf::Packet& packet)
{
    for (int i = 0; i < _clients.size(); i++)
    {
        sendToClient(packet, _clients[i]);
    }
}

void NetworkManager::addToUpdatePacket(sf::Packet& packet)
{
    _updatePacket.append(packet.getData(), packet.getDataSize());
}

void NetworkManager::serverDestroyEntity(size_t entID)
{
    sf::Packet pack;
    pack << DESTROY_ENT << entID;
    sendToAllClients(pack);
    sendToAllClients(pack);
    sendToAllClients(pack);
    // send 3 destroys in case one misses
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
        while (packet >> pType)
        {
            if (isServer)
            {
                if (pType == CONNECT_REQUEST)
                {
                    unsigned short clientPort;
                    packet >> clientPort;
                    std::cout << "Client " << sender.getPublicAddress() << " connected using port " << clientPort << std::endl;
                    _clients.push_back(Connection(sender, clientPort));
                    sf::Packet pack;
                    pack << CONNECT_CONFIRM << sender.getPublicAddress().toString();
                    sendToClient(pack, _clients[_clients.size() - 1]);
                }
            }
            if (isClient)
            {
                if (pType == CONNECT_CONFIRM)
                {
                    _engineRef->onClientConnected();
                }
                else if (pType == SCENE_EVENT)
                {
                    _engineRef->handleScenePacket(packet);
                }
                else if (pType == TRANSFORM)
                {
                    _engineRef->handleTransformPacket(packet);
                }
                else if (pType == DESTROY_ENT)
                {
                    size_t id;
                    packet >> id;
                    _engineRef->networkDestroy(id);
                }
            }
        }
    }
}

void NetworkManager::update(float dt)
{
    if (isServer)
    {
        _networkTimer += dt;
        if (_networkTimer >= NETWORK_SEND_RATE)
        {
            sendToAllClients(_updatePacket);
            _updatePacket.clear();
            _networkTimer = 0;
        }
    }
}


