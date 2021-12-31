#include "GameEngine.h"
#include "Resources/Assets.h"
#include "Framework/Scene.h"

void GameEngine::init(std::string gameName, unsigned int windowWidth, unsigned int windowHeight, std::string resourcePath)
{
    _window.create(sf::VideoMode(windowWidth, windowHeight), gameName);
    //_window.setFramerateLimit(240);
    
    // load resources
    _assets = std::make_shared<Assets>();
    _assets->loadAssets(resourcePath);
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

void GameEngine::changeScene(std::string name)
{
    if (_scenesMap.find(name) != _scenesMap.end())
    {
        _currentScene = _scenesMap[name];
        _currentScene->setEngineRefs(this, &_window);
        _currentScene->init();
    }
}

void GameEngine::update()
{
    sf::Time deltaTime = _updateTimer.restart();

    // handle events
    sUserInput();

    if (_currentScene != nullptr)
    {
        _currentScene->update(deltaTime.asSeconds());

        _window.clear();
        _currentScene->sRender();
        _window.display();
    }
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
        if (currEvent.type == sf::Event::KeyPressed)
        {
            //if (currEvent.key.code == sf::Keyboard::P) _debugToggle = !_debugToggle;
            if (currEvent.key.code == sf::Keyboard::Escape) quit();
        }
    }
}
