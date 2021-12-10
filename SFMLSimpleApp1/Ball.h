#pragma once
#include "GameObject.h"

class Ball : public GameObject
{
public:
	Ball(sf::Vector2f startPos, float radius, sf::RenderWindow * window, sf::Texture& texture);

	void update(float dt) override;
	void render() override;

	void reverse() { _velocity.x *= -1; }
	sf::FloatRect& getCollisionRect() { return _collisionRect; }

private:
	sf::Vector2f _velocity;
	sf::Sprite _sprite;
	sf::FloatRect _collisionRect;
	float _speed;
};

