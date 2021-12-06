#pragma once
#include "GameObject.h"
#include <string>
#include <sstream>

class Scoreboard : public GameObject
{
public:
	Scoreboard(sf::Font& font, int charSize, sf::RenderWindow* window);

	void update(float dt) override;
	void render() override;

private:
	sf::Text _player1Text;
	sf::Text _player2Text;
	int _player1Score = 0;
	int _player2Score = 0;
	std::stringstream _player1SS;
	std::stringstream _player2SS;
};

