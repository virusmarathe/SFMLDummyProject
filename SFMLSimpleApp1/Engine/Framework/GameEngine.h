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
		MOUSE_LEFT_UP,
		MOUSE_RIGHT_DOWN,
		MOUSE_RIGHT_UP,
		MOUSE_MIDDLE_DOWN,
		MOUSE_MIDDLE_UP
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

	std::shared_ptr<Assets> getAssets() { return _assets; }
	static bool DEBUG_MODE;

private:

	void update();
	void sUserInput();

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
};

