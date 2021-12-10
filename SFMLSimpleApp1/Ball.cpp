#include "Ball.h"
#include "Game.h"
#include "Config.h"

Ball::Ball(sf::Vector2f startPos, float radius, sf::RenderWindow * window, sf::Texture& texture)
{
	_window = window;
    _velocity = sf::Vector2f(1, 1);
    _speed = BALL_START_SPEED;
    _sprite.setTexture(texture);
    _sprite.setScale((radius * 2) / texture.getSize().x, (radius*2) / texture.getSize().y);
    _sprite.setPosition(startPos);
}

void Ball::update(float dt)
{
    sf::Vector2f curPos = _sprite.getPosition();
    _sprite.setPosition(curPos + _velocity * _speed * dt);

    if (curPos.x + _sprite.getGlobalBounds().width > _window->getSize().x)
    {
        _sprite.setPosition(sf::Vector2f(_window->getSize().x / 2.0f, _window->getSize().y / 2.0f));
        Game::instance().notifyBallScored(1);
        _speed = BALL_START_SPEED;
    }
    else if (curPos.x < 0)
    {
        _sprite.setPosition(sf::Vector2f(_window->getSize().x / 2.0f, _window->getSize().y / 2.0f));
        Game::instance().notifyBallScored(2);
        _speed = BALL_START_SPEED;
    }
    if (curPos.y + _sprite.getGlobalBounds().height > _window->getSize().y)
    {
        _velocity.y = -1;
        _speed += SPEED_GAIN_PER_HIT;
    }
    else if (curPos.y < 0)
    {
        _velocity.y = 1;
        _speed += SPEED_GAIN_PER_HIT;
    }

    _collisionRect = _sprite.getGlobalBounds();
}

void Ball::render()
{
	_window->draw(_sprite);
}
