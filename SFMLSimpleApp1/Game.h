#pragma once
#include "SFML/Graphics.hpp"
#include "Engine/Entity/EntityManager.h"
#include <vector>

class Entity;

class Game
{
public:

	static Game& instance() 
	{
		static Game* instance = new Game();
		return *instance;
	}

	void init(sf::RenderWindow * window);
	void handleInput();
	void update(float dt);
	void render();

	void notifyBallScored(int playerNum);

	void spawnNewBall();

private:
	Game() {}
	bool loadResources();
	void sPhysics(float dt);

	sf::RenderWindow * _window = nullptr;
	sf::Font _font;
	sf::Texture _ballTexture, _paddleTexture;
	std::shared_ptr<Entity> _player1Entity, _player2Entity, _player1Score, _player2Score;
	EntityManager _entities;
	bool _debugToggle = false;
	float _ballTimer = 0;
};

