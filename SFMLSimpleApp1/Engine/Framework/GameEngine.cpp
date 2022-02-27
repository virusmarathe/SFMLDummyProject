#include "GameEngine.h"
#include "Resources/Assets.h"
#include "Framework/Scene.h"
#include "Framework/Action.h"
#include "../../Game/Settings.h"
#include <iostream>
#include "Primitives.h"

bool GameEngine::DEBUG_MODE = false;

void GameEngine::init(std::string gameName, unsigned int windowWidth, unsigned int windowHeight, std::string resourcePath)
{
    _window.create(sf::VideoMode(windowWidth, windowHeight), gameName);
    //_window.setFramerateLimit(144);
    _window.setKeyRepeatEnabled(false);
    
    // load resources
    _assets = std::make_shared<Assets>();
    _assets->loadAssets(resourcePath);

    _networkManager.init(this);
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
        _fpsEnt = Primitives::DrawText("FPS: 0", Vector2(_window.getSize().x - 200, 200), _assets->getFont("NormalUIFont"));
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

void GameEngine::host()
{
    _networkManager.host();
    _isNetworked = true;
}

void GameEngine::connect()
{
    _networkManager.connect();
    _isNetworked = true;
}

void GameEngine::sendToAllClients(sf::Packet& packet)
{
    _networkManager.sendToAllClients(packet);
}

void GameEngine::sendToServer(sf::Packet& packet)
{
    _networkManager.addToUpdatePacket(packet);
}

void GameEngine::updatePacket(sf::Packet& packet)
{
    _networkManager.addToUpdatePacket(packet);
}

void GameEngine::addLateConnectPacket(sf::Packet& packet)
{
    _networkManager.addToLateConnectPacket(packet);
}

void GameEngine::handleScenePacket(sf::Packet& packet)
{
    _currentScene->handlePacket(packet);
}

void GameEngine::onClientReady(int id)
{
    _clientID = id;
    _currentScene->onClientReady(_clientID);
}

void GameEngine::onClientConnectedToServer(int clientID)
{
    _currentScene->onClientConnectedToServer(clientID);
}

void GameEngine::update()
{
    if (_isNetworked)
    {
        _networkManager.receive();
    }
    _currentScene->updateEntityList();
    fpsUpdate();
    sf::Time deltaTime = _updateTimer.restart();

    if (_currentScene != nullptr)
    {
        _currentScene->preUpdate(deltaTime.asSeconds());
        for (auto system : _systems)
        {
            system->update(deltaTime.asSeconds());
        }

        if (_isNetworked)
        {
            _networkManager.update(deltaTime.asSeconds());
        }

        _currentScene->postUpdate(deltaTime.asSeconds());
    }

    for (auto ent : _currentScene->getEntityManager()->getEntities("CollisionEvent"))
    {
        ent.destroy();
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

            _currentScene->sDoAction(Action(_actionMap[currEvent.key.code], aType, Vector2()));
        }
        if (currEvent.type == sf::Event::MouseWheelMoved)
        {
            int code = currEvent.mouseWheel.delta > 0 ? MOUSE_SCROLL_UP : MOUSE_SCROLL_DOWN;

            if (_actionMap.find(code) == _actionMap.end()) continue;          

            _currentScene->sDoAction(Action(_actionMap[code], Action::ActionType::START, Vector2()));
        }
        if (currEvent.type == sf::Event::MouseButtonPressed)
        {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(_window);
            Vector2 mousePos = (_window).mapPixelToCoords(pixelPos);
            int code;
            if (currEvent.mouseButton.button == sf::Mouse::Left) code = MOUSE_LEFT_DOWN;
            if (currEvent.mouseButton.button == sf::Mouse::Right) code = MOUSE_RIGHT_DOWN;
            if (currEvent.mouseButton.button == sf::Mouse::Middle) code = MOUSE_MIDDLE_DOWN;
            _currentScene->sDoAction(Action(_actionMap[code], Action::ActionType::START, mousePos));
        }
        if (currEvent.type == sf::Event::MouseMoved)
        {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(_window);
            Vector2 mousePos = (_window).mapPixelToCoords(pixelPos);
            _currentScene->sDoAction(Action(_actionMap[MOUSE_MOVED], Action::ActionType::START, mousePos));
        }
        if (currEvent.type == sf::Event::MouseButtonReleased)
        {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(_window);
            Vector2 mousePos = (_window).mapPixelToCoords(pixelPos);
            int code;
            if (currEvent.mouseButton.button == sf::Mouse::Left) code = MOUSE_LEFT_DOWN;
            if (currEvent.mouseButton.button == sf::Mouse::Right) code = MOUSE_RIGHT_DOWN;
            if (currEvent.mouseButton.button == sf::Mouse::Middle) code = MOUSE_MIDDLE_DOWN;
            _currentScene->sDoAction(Action(_actionMap[code], Action::ActionType::END, mousePos));
        }
    }
}

void GameEngine::fpsUpdate()
{
    FPS = (int)(1000000.0 / _updateTimer.getElapsedTime().asMicroseconds());
    Vector2 pos;
    for (auto ent : _currentScene->getEntityManager()->getEntities("Camera"))
    {
        auto& transform = ent.getComponent<CTransform>();
        pos = transform.position + Vector2(400, -200);
    }
    _fpsEnt.getComponent<CTransform>().position = pos;

    _fpsUpdateTimer += _updateTimer.getElapsedTime().asSeconds();
    if (_fpsUpdateTimer >= 0.1f)
    {
        _fpsEnt.getComponent<CText>().text.setString(sf::String("FPS: " + std::to_string(FPS)));
        _fpsUpdateTimer = 0;
    }
}
