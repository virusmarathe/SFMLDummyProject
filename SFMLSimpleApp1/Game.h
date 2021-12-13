#pragma once
#include "SFML/Graphics.hpp"
#include <vector>

class Ball;
class Paddle;
class Scoreboard;
class GameObject;
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

	bool hasCollision(const sf::FloatRect& shape1, const sf::FloatRect& shape2);

	void notifyBallScored(int playerNum);

	~Game();

private:
	Game() {}
	bool loadResources();

	sf::RenderWindow * _window = nullptr;
	Ball* _ball = nullptr;
	Paddle* _player1Paddle = nullptr;
	Paddle* _player2Paddle = nullptr;
	sf::Font _font;
	Scoreboard* _scoreboard = nullptr;
	std::vector<GameObject*> _gameObjects;
	sf::Texture _ballTexture, _paddleTexture;
	std::shared_ptr<Entity> _ballEntity, _player1Entity, _player2Entity;
	std::vector<std::shared_ptr<Entity>> _entities;
	bool _debugToggle = false;
};

