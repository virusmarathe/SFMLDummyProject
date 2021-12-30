#pragma once

#include "SFML/Graphics.hpp"
#include <string>

class Assets;

class GameEngine
{
public:
	GameEngine() : _isRunning(false) {}

	void init(std::string gameName, unsigned int windowWidth, unsigned int windowHeight, std::string resourcePath);
	void run();
	void quit();

	std::shared_ptr<Assets> getAssets() { return _assets; }

private:

	void update();
	void sUserInput();

	sf::RenderWindow _window;
	bool _isRunning;
	sf::Clock _updateTimer;
	std::shared_ptr<Assets> _assets;
};

