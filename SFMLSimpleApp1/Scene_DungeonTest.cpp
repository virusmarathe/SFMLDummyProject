#include "Scene_DungeonTest.h"
#include "Framework/GameEngine.h"
#include "Resources/Assets.h"
#include "Framework/Action.h"
#include "Config.h"
#include "System/SRender.h"
#include "System/SMovement.h"
#include "System/SAnimation.h"
#include "System/SPhysics.h"

void Scene_DungeonTest::init()
{
    _assets = _engine->getAssets();

    Rect groundRect(-5000, -5000, 10000, 10000);
    std::shared_ptr<Entity> ground = _entities.addEntity("Ground");
    ground->addComponent<CTransform>(groundRect.pos);
    std::shared_ptr<CSprite> groundSpriteComp = ground->addComponent<CSprite>(_assets->getTexture("Ground"));
    groundSpriteComp->sprite.setTextureRect(sf::IntRect(0, 0, 15000, 15000));

    _camera = _entities.addEntity("Camera");
    _camera->addComponent<CTransform>(Vector2(), Vector2(_window->getSize()));
    _camera->addComponent<CPhysicsBody>();

    _engine->registerAction(sf::Keyboard::P, "PHYSICS_TOGGLE");
    _engine->registerAction(sf::Keyboard::W, "P1UP");
    _engine->registerAction(sf::Keyboard::A, "P1LEFT");
    _engine->registerAction(sf::Keyboard::S, "P1DOWN");
    _engine->registerAction(sf::Keyboard::D, "P1RIGHT");
    _engine->registerAction(GameEngine::MOUSE_SCROLL_UP, "ZOOM_IN");
    _engine->registerAction(GameEngine::MOUSE_SCROLL_DOWN, "ZOOM_OUT");

    _engine->registerSystem(std::make_shared<SPhysics>(&_entities, Priority::PHYSICS));
    _engine->registerSystem(std::make_shared<SMovement>(&_entities, Priority::UPDATE));
    _engine->registerSystem(std::make_shared<SRender>(&_entities, Priority::RENDER, _window));
}

void Scene_DungeonTest::update(float dt)
{    
}

void Scene_DungeonTest::sDoAction(const Action& action)
{
    if (action.name == "PHYSICS_TOGGLE" && action.type == Action::ActionType::START) GameEngine::DEBUG_MODE = !GameEngine::DEBUG_MODE;

    float scale = _camera->getComponent<CTransform>()->scale.x / _window->getSize().x;

    if (action.name == "P1UP") _camera->getComponent<CPhysicsBody>()->velocity.y = action.type == Action::ActionType::START? -200 * scale : 0;
    if (action.name == "P1LEFT") _camera->getComponent<CPhysicsBody>()->velocity.x = action.type == Action::ActionType::START ? -200 * scale : 0;
    if (action.name == "P1DOWN") _camera->getComponent<CPhysicsBody>()->velocity.y = action.type == Action::ActionType::START ? 200 * scale : 0;
    if (action.name == "P1RIGHT") _camera->getComponent<CPhysicsBody>()->velocity.x = action.type == Action::ActionType::START ? 200 * scale : 0;

    if (action.name == "ZOOM_IN") _camera->getComponent<CTransform>()->scale /= 1.05f;
    if (action.name == "ZOOM_OUT") _camera->getComponent<CTransform>()->scale *= 1.05f;
}
