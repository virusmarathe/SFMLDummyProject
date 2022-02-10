#include "Scene_AnimationTest.h"
#include "Framework/GameEngine.h"
#include "Resources/Assets.h"
#include "Framework/Action.h"
#include "Config.h"
#include "System/SRender.h"
#include "System/SMovement.h"
#include "System/SAnimation.h"
#include "System/SPhysics.h"

void Scene_AnimationTest::init()
{
	_assets = _engine->getAssets();

    Rect groundRect(-5000, -5000, 10000, 10000);
    std::shared_ptr<Entity> ground = _entities.addEntity("Ground");
    ground->addComponent<CTransform>(groundRect.pos);
    std::shared_ptr<CSprite> groundSpriteComp = ground->addComponent<CSprite>(_assets->getTexture("Ground"));
    groundSpriteComp->sprite.setTextureRect(sf::IntRect(0, 0, 15000,15000));

    float gridSize = 50.0f;
    Rect wallRect(0, 0, (float)_window->getSize().x, gridSize);
    std::shared_ptr<Entity> wall = _entities.addEntity("Wall");
    wall->addComponent<CTransform>(wallRect.pos);
    wall->addComponent<CRectCollider>(wallRect);
    std::shared_ptr<CSprite> spriteComp = wall->addComponent<CSprite>(_assets->getTexture("Wall"));
    sf::IntRect texRect = spriteComp->sprite.getTextureRect();
    Vector2 scale(gridSize / texRect.width, gridSize / texRect.height);
    spriteComp->sprite.setScale(scale.x, scale.y);
    spriteComp->sprite.setTextureRect(sf::IntRect(0,0, (int)(wallRect.size.x / scale.x), (int)(wallRect.size.y / scale.y)));

    _camera = _entities.addEntity("Camera");
    _camera->addComponent<CTransform>(Vector2(), Vector2(_window->getSize()));

    _player1Entity = spawnPlayer();

    _engine->registerAction(sf::Keyboard::P, "PHYSICS_TOGGLE");
    _engine->registerAction(sf::Keyboard::Num2, "PHYSICS_SCENE");
    _engine->registerAction(sf::Keyboard::W, "P1UP");
    _engine->registerAction(sf::Keyboard::A, "P1LEFT");
    _engine->registerAction(sf::Keyboard::S, "P1DOWN");
    _engine->registerAction(sf::Keyboard::D, "P1RIGHT");
    _engine->registerAction(sf::Keyboard::LShift, "P1RUN");
    _engine->registerAction(GameEngine::MOUSE_SCROLL_UP, "ZOOM_IN");
    _engine->registerAction(GameEngine::MOUSE_SCROLL_DOWN, "ZOOM_OUT");

    _engine->playBGMusic("Level1BG");

    _engine->registerSystem(std::make_shared<SPhysics>(&_entities, Priority::PHYSICS));
    _engine->registerSystem(std::make_shared<SMovement>(&_entities, Priority::UPDATE));
    _engine->registerSystem(std::make_shared<SAnimation>(&_entities, Priority::UPDATE, _assets));
    _engine->registerSystem(std::make_shared<SRender>(&_entities, Priority::RENDER, _window));
}

void Scene_AnimationTest::update(float dt)
{
    _camera->getComponent<CTransform>()->position = _player1Entity->getComponent<CTransform>()->position;
}

void Scene_AnimationTest::sDoAction(const Action& action)
{
    if (action.name == "PHYSICS_TOGGLE" && action.type == Action::ActionType::START) GameEngine::DEBUG_MODE = !GameEngine::DEBUG_MODE;

    if (action.name == "PHYSICS_SCENE" && action.type == Action::ActionType::START) _engine->changeScene("PhysicsTest");

    if (action.name == "P1UP") _player1Entity->getComponent<CInput>()->up = action.type == Action::ActionType::START;
    if (action.name == "P1LEFT") _player1Entity->getComponent<CInput>()->left = action.type == Action::ActionType::START;
    if (action.name == "P1DOWN") _player1Entity->getComponent<CInput>()->down = action.type == Action::ActionType::START;
    if (action.name == "P1RIGHT") _player1Entity->getComponent<CInput>()->right = action.type == Action::ActionType::START;
    if (action.name == "P1RUN") _player1Entity->getComponent<CInput>()->run = action.type == Action::ActionType::START;

    if (action.name == "ZOOM_IN") _camera->getComponent<CTransform>()->scale /= 1.05f;
    if (action.name == "ZOOM_OUT") _camera->getComponent<CTransform>()->scale *= 1.05f;

    sInput();
}

std::shared_ptr<Entity> Scene_AnimationTest::spawnPlayer()
{
    auto player = _entities.addEntity("Player");
    player->addComponent<CTransform>(Vector2(200,200));
    std::shared_ptr<CSprite> sprite = player->addComponent<CSprite>(_assets->getTexture("ArcherWalk"));
    sprite->sprite.setScale(0.25f, 0.25f);
    std::shared_ptr<CAnimation> anim = player->addComponent<CAnimation>("ArcherWalk");
    player->addComponent<CRectCollider>(Rect(0,0,70,80), Vector2(20,5));
    player->addComponent<CPhysicsBody>();
    player->addComponent<CInput>();
    player->addComponent<CPhysicsAnimator>("ArcherIdle", "ArcherWalk", "ArcherRun", ARCHER_SPEED);

    return player;

}

void Scene_AnimationTest::sInput()
{
    for (auto ent : _entities.getEntities("Player"))
    {
        if (ent->hasComponent<CInput>() && ent->hasComponent<CPhysicsBody>())
        {
            std::shared_ptr<CInput> input = ent->getComponent<CInput>();
            Vector2 vel;

            if (input->up)     vel.y -= ARCHER_SPEED;
            if (input->down)   vel.y += ARCHER_SPEED;
            if (input->left)   vel.x -= ARCHER_SPEED;
            if (input->right)  vel.x += ARCHER_SPEED;

            if (input->run) vel *= 2;

            ent->getComponent<CPhysicsBody>()->velocity = vel;
        }
    }
}
