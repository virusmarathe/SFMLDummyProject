#include "Paddle.h"
#include "Config.h"

Paddle::Paddle(sf::Color color, Vector2 startPos, sf::RenderWindow * window)
{
	_shape.setFillColor(color);
	_position = startPos;
	_shape.setSize(sf::Vector2f(PADDLE_WIDTH, PADDLE_HEIGHT));
	_speed = PADDLE_SPEED;
	_window = window;
}

void Paddle::update(float dt)
{
	_position += _velocity * _speed * dt;

	if (_position.y < 0)
	{
		_position.y = 0;
	}
	if (_position.y + _shape.getLocalBounds().height > _window->getSize().y)
	{
		_position.y = _window->getSize().y - _shape.getLocalBounds().height;
	}

	_shape.setPosition(_position.x, _position.y);
	_collisionRect = _shape.getGlobalBounds();
}

void Paddle::render()
{
	_window->draw(_shape);
}
