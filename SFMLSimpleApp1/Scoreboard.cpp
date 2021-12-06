#include "Scoreboard.h"

Scoreboard::Scoreboard(sf::Font& font, int charSize, sf::RenderWindow * window)
{
	_window = window;
	_player1Text.setFont(font);
	_player2Text.setFont(font);
	_player1Text.setCharacterSize(24);
	_player2Text.setCharacterSize(24);
	_player1Text.setFillColor(sf::Color::Blue);
	_player2Text.setFillColor(sf::Color::Red);
	_player1Text.setString("Player 1: 0");
	_player2Text.setString("Player 2: 0");
	_player1Text.setPosition(sf::Vector2f(_window->getSize().x / 2.0f - _player1Text.getLocalBounds().width / 2.0f, 20));
	_player2Text.setPosition(sf::Vector2f(_window->getSize().x / 2.0f - _player2Text.getLocalBounds().width / 2.0f, 60));
}

void Scoreboard::update(float dt)
{
	_player1SS.clear();
	_player1SS.str("");
	_player2SS.clear();
	_player2SS.str("");
	_player1SS << "Player 1: " << _player1Score;
	_player2SS << "Player 2: " << _player2Score;
	_player1Text.setString(_player1SS.str());
	_player2Text.setString(_player2SS.str());
}

void Scoreboard::render()
{
	_window->draw(_player1Text);
	_window->draw(_player2Text);
}
