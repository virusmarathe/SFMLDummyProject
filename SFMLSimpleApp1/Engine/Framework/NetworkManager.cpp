#include "NetworkManager.h"
#include "../../Game/Settings.h"
#include <iostream>
#include "Math/Vector2.h"
#include "GameEngine.h"
#include "Entity/Entity.h"

#define LOCAL_TEST 1

bool NetworkManager::isClient = false;
bool NetworkManager::isServer = false;
unsigned int NetworkManager::nextNetID = 0;
int NetworkManager::nextClientID = 0;
int NetworkManager::clientID = -1;

void NetworkManager::init(GameEngine* engineRef)
{
    _engineRef = engineRef;
}

void NetworkManager::host()
{
    isServer = true;
    isClient = true;
    clientID = nextClientID++;
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

void NetworkManager::addToLateConnectPacket(sf::Packet& packet)
{
    _lateConnectPacket.append(packet.getData(), packet.getDataSize());
}

void NetworkManager::serverDestroyEntity(unsigned int netID)
{
    _netIDToEntityMap[netID].destroy();
    _netIDToEntityMap.erase(netID);
    sf::Packet pack;
    pack << DESTROY_ENT << netID;
    sendToAllClients(pack);
    sendToAllClients(pack);
    sendToAllClients(pack);
    // send 3 destroys in case one misses
}

Entity NetworkManager::serverCreateEntity(EntityManager* entities, std::string tag)
{
    auto ent = entities->addEntity(tag);
    unsigned int netID = nextNetID++;
    auto& comp = ent.addComponent<CNetID>();
    comp.netID = netID;
    _netIDToEntityMap[netID] = ent;
    return ent;
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
                    int id = nextClientID++;
                    std::cout << "Client ID " << id << " with ip: " << sender.getPublicAddress() << " connected using port " << clientPort << std::endl;
                    _clients.push_back(Connection(sender, clientPort, id));
                    sf::Packet pack;
                    pack << CONNECT_CONFIRM << id;
                    sendToClient(pack, _clients[_clients.size() - 1]);
                    _engineRef->onClientConnectedToServer(id);
                }
                else if (pType == LATE_CONNECT_REQUEST)
                {
                    int id;
                    packet >> id;
                    for (int i = 0; i < _clients.size(); i++)
                    {
                        if (_clients[i].clientID == id)
                        {
                            sendToClient(_lateConnectPacket, _clients[i]);
                        }
                    }
                }
                else if (pType == INPUT_EVENT)
                {
                    unsigned int netID;
                    packet >> netID;
                    if (_netIDToEntityMap.find(netID) != _netIDToEntityMap.end())
                    {
                        Entity ent = _netIDToEntityMap[netID];
                        packet >> ent.getComponent<CInput>();
                    }
                }
            }
            if (isClient)
            {
                if (pType == CONNECT_CONFIRM)
                {
                    if (_connected) break;

                    packet >> clientID;
                    _engineRef->onClientReady(clientID);
                    _connected = true;
                }
                else if (pType == SCENE_EVENT)
                {
                    _engineRef->handleScenePacket(packet);
                }
                else if (pType == TRANSFORM)
                {
                    unsigned int netID;
                    Vector2 pos, vel;
                    packet >> netID >> pos >> vel;
                    if (_netIDToEntityMap.find(netID) != _netIDToEntityMap.end())
                    {
                        Entity ent = _netIDToEntityMap[netID];
                        ent.getComponent<CNetworkTransform>().position = pos;
                        ent.getComponent<CNetworkTransform>().velocity = vel;
                    }
                }
                else if (pType == DESTROY_ENT)
                {
                    unsigned int id;
                    packet >> id;
                    if (_netIDToEntityMap.find(id) != _netIDToEntityMap.end())
                    {
                        _netIDToEntityMap[id].destroy();
                        _netIDToEntityMap.erase(id);
                    }
                }
            }
        }
    }
}

void NetworkManager::update(float dt)
{
        _networkTimer += dt;
        if (_networkTimer >= NETWORK_SEND_RATE)
        {
            if (isServer)
            {
                sendToAllClients(_updatePacket);
                _updatePacket.clear();
            }
            else
            {
                sendToServer(_updatePacket);
                _updatePacket.clear();
            }
            _networkTimer = 0;
        }
}


