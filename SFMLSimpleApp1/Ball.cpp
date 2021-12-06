#include "Ball.h"

Ball::Ball(sf::Vector2f startPos, float radius, sf::RenderWindow * window)
{
	_shape.setFillColor(sf::Color::Green);
	_shape.setPosition(startPos);
	_shape.setRadius(radius);
	_window = window;
    _velocity = sf::Vector2f(1, 1);
    _speed = 200;
}

void Ball::update(float dt)
{
    sf::Vector2f curPos = _shape.getPosition();
    _shape.setPosition(curPos + _velocity * _speed * dt);

    if (curPos.x + _shape.getLocalBounds().width > _window->getSize().x)
    {
        _shape.setPosition(sf::Vector2f(_window->getSize().x / 2.0f, _window->getSize().y / 2.0f));
        //blueScore++;
        _speed = 200;
    }
    else if (curPos.x < 0)
    {
        _shape.setPosition(sf::Vector2f(_window->getSize().x / 2.0f, _window->getSize().y / 2.0f));
        //redScore++;
        _speed = 200;
    }
    if (curPos.y + _shape.getLocalBounds().height > _window->getSize().y)
    {
        _velocity.y = -1;
        _speed += 10;
    }
    else if (curPos.y < 0)
    {
        _velocity.y = 1;
        _speed += 10;
    }
}

void Ball::render()
{
	_window->draw(_shape);
}
