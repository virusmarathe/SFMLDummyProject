#pragma once
#include "GameObject.h"

class Paddle : public GameObject
{
public:
	Paddle(sf::Color color, sf::Vector2f startPos, sf::RenderWindow * windowRef);
	void update(float dt) override;
	void render() override;

	void SetVelocity(float velY) { _velocity.y = velY; }

	sf::RectangleShape& getShape() { return _shape; }

private:
	sf::RectangleShape _shape;
	sf::Vector2f _velocity;
	float _speed;
};

