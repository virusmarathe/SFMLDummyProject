#pragma once

#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include <string>
#include <map>
#include <vector>
#include "System/System.h"
#include "NetworkManager.h"

class Assets;
class Scene;

class GameEngine
{
public:
	GameEngine() : _isRunning(false) {}

	enum MouseKeyEvent
	{
		MOUSE_INVALID = sf::Keyboard::KeyCount,
		MOUSE_SCROLL_UP,
		MOUSE_SCROLL_DOWN,
		MOUSE_LEFT_DOWN,
		MOUSE_RIGHT_DOWN,
		MOUSE_MIDDLE_DOWN,
		MOUSE_MOVED
	};

	void init(std::string gameName, unsigned int windowWidth, unsigned int windowHeight, std::string resourcePath);
	void run();
	void quit();
	void registerScene(std::string name, std::shared_ptr<Scene> scene) { _scenesMap[name] = scene; }
	void changeScene(std::string name);
	void registerAction(int key, std::string name) { _actionMap[key] = name; }
	void playBGMusic(std::string name);
	void playSound(std::string name);
	void registerSystem(std::shared_ptr<System> system) { _systems.push_back(system); }

	// networking
	void host();
	void connect();
	void sendToAllClients(sf::Packet& packet);
	void sendToServer(sf::Packet& packet);
	void updatePacket(sf::Packet& packet);
	void addLateConnectPacket(sf::Packet& packet);
	void handleScenePacket(sf::Packet& packet);
	NetworkManager * getNetManager() { return &_networkManager; }

	// events
	void onClientReady(int id);
	void onClientConnectedToServer(int clientID);

	std::shared_ptr<Assets> getAssets() { return _assets; }
	static bool DEBUG_MODE;
	int FPS = 0;

private:

	void update();
	void sUserInput();
	void fpsUpdate();

	sf::RenderWindow _window;
	bool _isRunning;
	sf::Clock _updateTimer;
	std::shared_ptr<Assets> _assets;
	std::map<std::string, std::shared_ptr<Scene>> _scenesMap;
	std::shared_ptr<Scene> _currentScene;
	std::map<int, std::string> _actionMap;
	sf::Music * _currentMusic = nullptr;
	sf::Sound _soundPool[10];
	int _soundIndex = 0;
	std::vector<std::shared_ptr<System>> _systems;
	NetworkManager _networkManager;
	bool _isNetworked = false;
	int _clientID = -1;
	Entity _fpsEnt;
	float _fpsUpdateTimer = 0;
};

