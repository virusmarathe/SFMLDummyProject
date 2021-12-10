#pragma once
#include "GameObject.h"

class Paddle : public GameObject
{
public:
	Paddle(sf::Color color, sf::Vector2f startPos, sf::RenderWindow * windowRef);
	void update(float dt) override;
	void render() override;

	void SetVelocity(float velY) { _velocity.y = velY; }

	sf::FloatRect& getCollisionRect() { return _collisionRect; }

private:
	sf::RectangleShape _shape;
	sf::Vector2f _velocity;
	sf::FloatRect _collisionRect;
	float _speed;
};

