#include "Paddle.h"

Paddle::Paddle(sf::Color color, sf::Vector2f startPos, sf::RenderWindow * window)
{
	_shape.setFillColor(color);
	_shape.setPosition(startPos);
	_shape.setSize(sf::Vector2f(10, 100));
	_speed = 400.0f;
	_window = window;
}

void Paddle::update(float dt)
{
	_shape.setPosition(_shape.getPosition() + _velocity * _speed * dt);

	if (_shape.getPosition().y < 0)
	{
		_shape.setPosition(sf::Vector2f(_shape.getPosition().x, 0));
	}
	if (_shape.getPosition().y + _shape.getLocalBounds().height > _window->getSize().y)
	{
		_shape.setPosition(sf::Vector2f(_shape.getPosition().x, _window->getSize().y - _shape.getLocalBounds().height));
	}
}

void Paddle::render()
{
	_window->draw(_shape);
}
