#include "Game.h"
#include "Paddle.h"
#include "Ball.h"
#include "Scoreboard.h"
#include <iostream>

void Game::init(sf::RenderWindow * window)
{
	_window = window;
    if (!_font.loadFromFile("BalooBhaijaan2-VariableFont_wght.ttf"))
    {
        std::cout << "Couldn't find font BalooBhaijaan2-VariableFont_wght.ttf";
    }

    _ball = new Ball(sf::Vector2f(_window->getSize().x / 2.0f, _window->getSize().y / 2.0f), 10.0f, _window);
    _player1Paddle = new Paddle(sf::Color::Red, sf::Vector2f(_window->getSize().x - 10.0f, 0), _window);
    _player2Paddle = new Paddle(sf::Color::Blue, sf::Vector2f(0, 0), _window);
    _scoreboard = new Scoreboard(_font, 24, _window);

    _gameObjects.push_back(_ball);
    _gameObjects.push_back(_player1Paddle);
    _gameObjects.push_back(_player2Paddle);
    _gameObjects.push_back(_scoreboard);

}

void Game::handleInput()
{
    sf::Event currEvent;
    while (_window->pollEvent(currEvent))
    {
        if (currEvent.type == sf::Event::Closed)
        {
            _window->close();
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))    _player1Paddle->SetVelocity(1);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))      _player1Paddle->SetVelocity(-1);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))       _player2Paddle->SetVelocity(-1);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))       _player2Paddle->SetVelocity(1);
}

void Game::update(float dt)
{
    if (hasCollision(_ball->getShape(), _player1Paddle->getShape()) || hasCollision(_ball->getShape(), _player2Paddle->getShape()))
    {
        _ball->reverse();
    }

    for (int i = 0; i < _gameObjects.size(); i++)
    {
        _gameObjects[i]->update(dt);
    }
}

void Game::render()
{
    _window->clear();

    for (int i = 0; i < _gameObjects.size(); i++)
    {
        _gameObjects[i]->render();
    }

    _window->display();
}

bool Game::hasCollision(const sf::Shape& shape1, const sf::Shape& shape2)
{
    bool foundCollision = false;

    sf::FloatRect shape1Rect = shape1.getGlobalBounds();
    sf::FloatRect shape2Rect = shape2.getGlobalBounds();

    if (shape1Rect.left < shape2Rect.left + shape2Rect.width && shape1Rect.left + shape1Rect.width > shape2Rect.left &&
        shape1Rect.top < shape2Rect.top + shape2Rect.height && shape1Rect.top + shape1Rect.height > shape2Rect.top) return true;

    return foundCollision;
}

void Game::notifyBallScored(int playerNum)
{
    _scoreboard->playerScored(playerNum);
}

Game::~Game()
{
    for (int i = 0; i < _gameObjects.size(); i++)
    {
        delete _gameObjects[i];
    }
    _gameObjects.clear();
}
