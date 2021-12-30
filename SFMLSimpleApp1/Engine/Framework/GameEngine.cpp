#include "GameEngine.h"
#include "../Game.h"
#include "Resources/Assets.h"

void GameEngine::init(std::string gameName, unsigned int windowWidth, unsigned int windowHeight, std::string resourcePath)
{
    _window.create(sf::VideoMode(windowWidth, windowHeight), gameName);
    _window.setFramerateLimit(240);

    // load resources
    _assets = std::make_shared<Assets>();
    _assets->loadAssets(resourcePath);

    Game::instance().init(&_window, this);
}

void GameEngine::run()
{
    _isRunning = true;
    while (_isRunning)
    {
        update();
    }
}

void GameEngine::quit()
{
    _window.close();
    _isRunning = false;
}

void GameEngine::update()
{
    // handle events
    sUserInput();

    sf::Time deltaTime = _updateTimer.restart();
    // update
    Game::instance().update(deltaTime.asSeconds());

    // render
    Game::instance().render();
}

void GameEngine::sUserInput()
{
    sf::Event currEvent;
    while (_window.pollEvent(currEvent))
    {
        if (currEvent.type == sf::Event::Closed)
        {
            quit();
        }
        /*if (currEvent.type == sf::Event::KeyPressed)
        {
            if (currEvent.key.code == sf::Keyboard::P) _debugToggle = !_debugToggle;
        }*/
    }
}
