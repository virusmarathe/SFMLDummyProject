#include "Ball.h"
#include "Game.h"

Ball::Ball(sf::Vector2f startPos, float radius, sf::RenderWindow * window, sf::Texture& texture)
{
	_shape.setFillColor(sf::Color::Green);
	_shape.setPosition(startPos);
	_shape.setRadius(radius);
	_window = window;
    _velocity = sf::Vector2f(1, 1);
    _speed = 200;
    _sprite.setTexture(texture);
}

void Ball::update(float dt)
{
    sf::Vector2f curPos = _shape.getPosition();
    _shape.setPosition(curPos + _velocity * _speed * dt);

    if (curPos.x + _shape.getLocalBounds().width > _window->getSize().x)
    {
        _shape.setPosition(sf::Vector2f(_window->getSize().x / 2.0f, _window->getSize().y / 2.0f));
        Game::instance().notifyBallScored(1);
        _speed = 200;
    }
    else if (curPos.x < 0)
    {
        _shape.setPosition(sf::Vector2f(_window->getSize().x / 2.0f, _window->getSize().y / 2.0f));
        Game::instance().notifyBallScored(2);
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

    _sprite.setPosition(_shape.getPosition());
}

void Ball::render()
{
	_window->draw(_shape);
    _window->draw(_sprite);
}
