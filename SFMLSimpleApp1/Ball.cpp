#include "Ball.h"
#include "Game.h"
#include "Config.h"

Ball::Ball(Vector2 startPos, float radius, sf::RenderWindow * window, sf::Texture& texture)
{
	_window = window;
    _position = startPos;
    _velocity = Vector2(1,1);
    _speed = BALL_START_SPEED;
    _sprite.setTexture(texture);
    _sprite.setScale((radius * 2) / texture.getSize().x, (radius*2) / texture.getSize().y);
}

void Ball::update(float dt)
{
    _position += _velocity * _speed * dt;

    if (_position.x + _sprite.getGlobalBounds().width > _window->getSize().x)
    {
        _position = Vector2(_window->getSize().x / 2.0f, _window->getSize().y / 2.0f);
        Game::instance().notifyBallScored(1);
        _speed = BALL_START_SPEED;
    }
    else if (_position.x < 0)
    {
        _position = Vector2(_window->getSize().x / 2.0f, _window->getSize().y / 2.0f);
        Game::instance().notifyBallScored(2);
        _speed = BALL_START_SPEED;
    }
    if (_position.y + _sprite.getGlobalBounds().height > _window->getSize().y)
    {
        _velocity.y = -1;
        _speed += SPEED_GAIN_PER_HIT;
    }
    else if (_position.y < 0)
    {
        _velocity.y = 1;
        _speed += SPEED_GAIN_PER_HIT;
    }

    _sprite.setPosition(_position.x, _position.y);
    _collisionRect = _sprite.getGlobalBounds();
}

void Ball::render()
{
	_window->draw(_sprite);
}
