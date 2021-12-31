#pragma once

#include "SFML/Graphics.hpp"
#include <string>
#include <map>

class Assets;
class Scene;

class GameEngine
{
public:
	GameEngine() : _isRunning(false) {}

	void init(std::string gameName, unsigned int windowWidth, unsigned int windowHeight, std::string resourcePath);
	void run();
	void quit();
	void registerScene(std::string name, std::shared_ptr<Scene> scene) { _scenesMap[name] = scene; }
	void changeScene(std::string name);

	std::shared_ptr<Assets> getAssets() { return _assets; }

private:

	void update();
	void sUserInput();

	sf::RenderWindow _window;
	bool _isRunning;
	sf::Clock _updateTimer;
	std::shared_ptr<Assets> _assets;
	std::map<std::string, std::shared_ptr<Scene>> _scenesMap;
	std::shared_ptr<Scene> _currentScene;
};

