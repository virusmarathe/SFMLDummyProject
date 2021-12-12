#pragma once
#include "GameObject.h"
#include "Engine/Math/Vector2.h"

class Paddle : public GameObject
{
public:
	Paddle(sf::Color color, Vector2 startPos, sf::RenderWindow * windowRef);
	void update(float dt) override;
	void render() override;

	void SetVelocity(float velY) { _velocity.y = velY; }

	sf::FloatRect& getCollisionRect() { return _collisionRect; }

private:
	sf::RectangleShape _shape;
	Vector2 _position;
	Vector2 _velocity;
	sf::FloatRect _collisionRect;
	float _speed;
};

