// SFMLSimpleApp1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include "Game.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Pong!");
    Game::instance().init(&window);

    sf::Clock deltaTimer;
    while (window.isOpen())
    {
        // handle events
        Game::instance().handleInput();

        // update
        sf::Time deltaTime = deltaTimer.restart();
        Game::instance().update(deltaTime.asSeconds());

        // render
        Game::instance().render();
    }

    return 0;
}
