#include "Scene.h"
#include "Framework/Primitives.h"

void Scene::setEngineRefs(GameEngine* engine, sf::RenderWindow* window)
{
	_engine = engine;
	_window = window;
	_entities.destroyAll();
	Primitives::_entities = &_entities;
}
