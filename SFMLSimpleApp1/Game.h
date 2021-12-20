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
private:
	Game() {}
	bool loadResources();
	void sInput();
	void sPhysics(float dt);
	void sMovement(float dt);
	void sRender();
	void sDebugDraw();

	void spawnNewBall();
	void spawnWall(Rect rect);
	void spawnPlayer(Vector2 pos, int playerNum);

	sf::RenderWindow * _window = nullptr;
	sf::Font _font;
	sf::Texture _ballTexture, _paddleTexture;
	std::shared_ptr<Entity> _player1Score, _player2Score;
	EntityManager _entities;
	bool _debugToggle = false;
	float _ballTimer = 0;
};

