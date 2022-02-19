#include "Scene_Menu.h"
#include "Framework/GameEngine.h"
#include "Framework/Primitives.h"
#include "System/SRender.h"
#include "Framework/Action.h"
#include <iostream>

void Scene_Menu::init()
{
	_assets = _engine->getAssets();
	Primitives::DrawText("Press H for host.", Vector2(_window->getSize().x / 2.0f - 100.0f, _window->getSize().y / 2.0f), _assets->getFont("NormalUIFont"));
	Primitives::DrawText("Press J to join a server.", Vector2(_window->getSize().x / 2.0f - 100.0f, _window->getSize().y / 2.0f + 50), _assets->getFont("NormalUIFont"));
	auto camera = _entities.addEntity("Camera");
	camera->addComponent<CTransform>(Vector2(_window->getSize().x / 2.0f, _window->getSize().y / 2.0f), Vector2(_window->getSize()));

	_engine->registerAction(sf::Keyboard::H, "Host");
	_engine->registerAction(sf::Keyboard::J, "Join");
	_engine->registerSystem(std::make_shared<SRender>(&_entities, Priority::RENDER, _window));
}

void Scene_Menu::update(float dt)
{

}

void Scene_Menu::sDoAction(const Action& action)
{
	if (action.name == "Host" && action.type == Action::ActionType::START)
	{
		std::cout << "Hosting a game!" << std::endl;
		_engine->host();
		_engine->changeScene("PhysicsTest");
	}
	if (action.name == "Join" && action.type == Action::ActionType::START)
	{
		std::cout << "Joining a game!" << std::endl;
		_engine->connect();
	}
}

void Scene_Menu::onClientConnected()
{
	_engine->changeScene("PhysicsTest");
}
