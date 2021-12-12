#pragma once
#include "GameObject.h"
#include "Engine/Math/Vector2.h"

class Ball : public GameObject
{
public:
	Ball(Vector2 startPos, float radius, sf::RenderWindow * window, sf::Texture& texture);

	void update(float dt) override;
	void render() override;

	void reverse() { _velocity.x *= -1; }
	sf::FloatRect& getCollisionRect() { return _collisionRect; }

private:
	Vector2 _position;
	Vector2 _velocity;
	sf::Sprite _sprite;
	sf::FloatRect _collisionRect;
	float _speed;
};

