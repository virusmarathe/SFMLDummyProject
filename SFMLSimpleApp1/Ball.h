#pragma once
#include "GameObject.h"

class Ball : public GameObject
{
public:
	Ball(sf::Vector2f startPos, float radius, sf::RenderWindow * window, sf::Texture& texture);

	void update(float dt) override;
	void render() override;

	void reverse() { _velocity.x *= -1; }
	sf::CircleShape& getShape() { return _shape; }

private:
	sf::CircleShape _shape;
	sf::Vector2f _velocity;
	sf::Sprite _sprite;
	float _speed;
};

