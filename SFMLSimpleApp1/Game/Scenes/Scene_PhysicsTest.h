#pragma once
#include "Framework/Scene.h"
#include "Resources/Assets.h"

class Scene_PhysicsTest : public Scene
{
public:

	virtual void init() override;

	virtual void update(float dt) override;

	virtual void sDoAction(const Action& action) override;

	virtual void handlePacket(sf::Packet& packet) override;

private:

	void sInput();
	void sHandleCollision(float dt);

	void spawnBallServer();
	void spawnBallClient(sf::Packet& packet);
	void spawnWall(Rect rect);
	std::shared_ptr<Entity> spawnPlayer(Vector2 pos, int playerNum);

	std::shared_ptr<Assets> _assets;
	float _ballTimer = 0;
	std::shared_ptr<Entity> _player1Entity;
	std::shared_ptr<Entity> _player2Entity;
	std::shared_ptr<Entity> _player1ScoreBoard;
	std::shared_ptr<Entity> _player2ScoreBoard;
	std::shared_ptr<Entity> _player1Goal;
	std::shared_ptr<Entity> _player2Goal;
	int _player1Score;
	int _player2Score;
};

