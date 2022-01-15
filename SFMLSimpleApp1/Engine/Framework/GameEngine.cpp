#include "GameEngine.h"
#include "Resources/Assets.h"
#include "Framework/Scene.h"
#include "Framework/Action.h"

bool GameEngine::DEBUG_MODE = false;

void GameEngine::init(std::string gameName, unsigned int windowWidth, unsigned int windowHeight, std::string resourcePath)
{
    _window.create(sf::VideoMode(windowWidth, windowHeight), gameName);
    _window.setFramerateLimit(144);
    _window.setKeyRepeatEnabled(false);
    
    // load resources
    _assets = std::make_shared<Assets>();
    _assets->loadAssets(resourcePath);
}

void GameEngine::run()
{
    _isRunning = true;
    while (_isRunning)
    {
        // handle events
        sUserInput();

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
        _actionMap.clear();
        _systems.clear();
        _currentScene = _scenesMap[name];
        _currentScene->setEngineRefs(this, &_window);
        _currentScene->init();
    }
}

void GameEngine::playBGMusic(std::string name)
{
    if (_currentMusic != nullptr)
    {
        _currentMusic->stop();
    }
    _currentMusic = &_assets->getMusic(name);
    _currentMusic->play();
}

void GameEngine::playSound(std::string name)
{
    _soundPool[_soundIndex].setBuffer(_assets->getSound(name));
    _soundPool[_soundIndex].play();
    _soundIndex++;
    if (_soundIndex >= 10) _soundIndex = 0;
}

void GameEngine::update()
{
    _currentScene->updateEntityList();
    sf::Time deltaTime = _updateTimer.restart();

    if (_currentScene != nullptr)
    {
        _currentScene->update(deltaTime.asSeconds());
        for (auto system : _systems)
        {
            system->update(deltaTime.asSeconds());
        }
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
        if (currEvent.type == sf::Event::KeyPressed || currEvent.type == sf::Event::KeyReleased)
        {
            if (_actionMap.find(currEvent.key.code) == _actionMap.end()) continue;

            Action::ActionType aType = currEvent.type == sf::Event::KeyPressed ? Action::ActionType::START : Action::ActionType::END;

            _currentScene->sDoAction(Action(_actionMap[currEvent.key.code], aType));
        }
    }
}