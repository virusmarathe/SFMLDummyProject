// SFMLSimpleApp1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include "Game.h"
#include "Config.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Pong!");
    window.setFramerateLimit(240);
    Game::instance().init(&window);

    sf::Clock deltaTimer;
    while (window.isOpen())
    {
        sf::Time deltaTime = deltaTimer.restart();

        // handle events
        Game::instance().handleInput();

        // update
        Game::instance().update(deltaTime.asSeconds());

        // render
        Game::instance().render();
    }

    return 0;
}
